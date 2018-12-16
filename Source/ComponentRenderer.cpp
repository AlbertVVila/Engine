#include "ComponentRenderer.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "GameObject.h"
#include "Imgui/imgui.h"
#include "GL/glew.h"

#include "assimp/material.h"
#include "JSON.h"
#include <vector>
ComponentRenderer::ComponentRenderer(GameObject* gameobject) : Component(gameobject, ComponentType::Renderer)
{
	material = new Material;
	material->shader = App->program->defaultShader;
}

ComponentRenderer::ComponentRenderer(const ComponentRenderer& component) : Component(component)
{
	//TODO: deep copy material pointer
}

ComponentRenderer::~ComponentRenderer()
{
	DeleteTexture();
}

Component * ComponentRenderer::Clone()
{
	return new ComponentRenderer(*this);
}

void ComponentRenderer::DeleteTexture()//TODO delete all textures of materials
{
	if (material != nullptr)
	{
		for (unsigned i = 0; i < MAXTEXTURES; i++)
		{
			if (material->textures[i] != nullptr)
			{
				RELEASE(material->textures[i]);
				glDeleteTextures(1, (GLuint*)&material->textures[i]->id);
			}
		}
	}
}

void ComponentRenderer::SetTexture(const char * newTexture, TextureType type)
{
	if (newTexture != nullptr)
	{
		material->SetTexture(App->textures->Load(newTexture), type);
	}
}

void ComponentRenderer::SetShader(const char * shaderName)
{
	if (shaderName != nullptr)
	{
		material->shader = App->program->GetProgram(shaderName); //TODO: refactor shader + texture + material
	}
}

Texture * ComponentRenderer::GetTexture(TextureType type) const
{
	return material->textures[(unsigned)type];
}

Shader * ComponentRenderer::GetShader() const
{
	return material->shader;
}

void ComponentRenderer::DrawProperties()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool removed = Component::DrawComponentState();
		if (removed)
		{
			ImGui::PopID();
			return;
		}
		std::vector<std::string> shaders = App->fsystem->ListFiles(VERTEXSHADERS, false);
		if (ImGui::BeginCombo("Shader", selected_shader.c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < shaders.size(); n++)
			{
				bool is_selected = (selected_shader == shaders[n]);
				if (ImGui::Selectable(shaders[n].c_str(), is_selected) && selected_shader != shaders[n])
				{
					selected_shader = shaders[n];
					material->shader = App->program->GetProgram(selected_shader.c_str());
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::Text((material->name + " Material").c_str());
		std::vector<std::string> textures = App->fsystem->ListFiles(TEXTURES, false);
		const char* textureTypes[] = { "Diffuse", "Specular", "Occlusion", "Emissive" };
		unsigned i = 0;
		for (auto &matTexture : material->textures)
		{
			ImGui::PushID(&matTexture);
			if (ImGui::BeginCombo(textureTypes[i], selected_texture[i].c_str())) 
			{
				for (int n = 0; n < textures.size(); n++)
				{
					bool is_selected = (selected_texture[i] == textures[n]);
					if (ImGui::Selectable(textures[n].c_str(), is_selected) && selected_texture[i] != textures[n])
					{
						selected_texture[i] = textures[n];
						matTexture = App->textures->Load(selected_texture[i].c_str());
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			if (matTexture != nullptr)
			{
				ImGui::Text("Texture width:%d height:%d", matTexture->width, matTexture->height);
				float size = ImGui::GetWindowWidth();
				ImGui::Image((ImTextureID)matTexture->id, { size,size }, { 0,1 }, { 1,0 });
				ImGui::Separator();
			}
			ImGui::PopID();
			++i;
		}
	}
	ImGui::PopID();
}

void ComponentRenderer::Save(JSON_value * value) const
{
	Component::Save(value);
	SaveMaterial();
	value->AddString("Material", material->name.c_str());
}

void ComponentRenderer::Load(JSON_value * value)
{
	Component::Load(value);
	const char* materialFile = value->GetString("Material");
	if (materialFile != nullptr)
	{
		LoadMaterial(materialFile);
	}
}

void ComponentRenderer::LoadMaterial(const char* materialfile)
{
	char* data = nullptr;
	std::string materialName(materialfile);
	App->fsystem->Load((MATERIALS + materialName + JSONEXT).c_str(), &data);
	JSON *json = new JSON(data);
	JSON_value *materialJSON = json->GetValue("material");

	material->name = materialName;
	material->diffuse_color = materialJSON->GetFloat4("diffuseColor");
	material->specular_color = materialJSON->GetFloat3("specularColor");
	material->emissive_color = materialJSON->GetFloat3("emissiveColor");

	material->kAmbient = materialJSON->GetFloat("kAmbient");
	material->kDiffuse = materialJSON->GetFloat("kDiffuse");
	material->kSpecular = materialJSON->GetFloat("kSpecular");
	material->shininess = materialJSON->GetFloat("shininess");

	JSON_value *textureJSON = materialJSON->GetValue("textures");
	if (textureJSON != nullptr)
	{
		material->textures[(unsigned)TextureType::DIFFUSE] = materialJSON->GetTexture("diffuse");
		material->textures[(unsigned)TextureType::SPECULAR] = materialJSON->GetTexture("specular");
		material->textures[(unsigned)TextureType::OCCLUSION] = materialJSON->GetTexture("occlusion");
		material->textures[(unsigned)TextureType::EMISSIVE] = materialJSON->GetTexture("emissive");
	}
	JSON_value *shaderJSON = materialJSON->GetValue("shader");
	if (shaderJSON != nullptr)
	{
		material->shader = new Shader(shaderJSON->GetUint("value"), shaderJSON->GetString("file"));
	}
}

void ComponentRenderer::SaveMaterial() const
{
	JSON *json = new JSON();
	JSON_value *materialJSON = json->CreateValue();

	if(material->textures[(unsigned) TextureType::DIFFUSE] != nullptr)
		materialJSON->AddFloat4("diffuseColor", material->diffuse_color);

	if (material->textures[(unsigned)TextureType::SPECULAR] != nullptr)
		materialJSON->AddFloat3("specularColor", material->specular_color);

	if (material->textures[(unsigned)TextureType::EMISSIVE] != nullptr)
		materialJSON->AddFloat3("emissiveColor", material->emissive_color);

	materialJSON->AddFloat("kAmbient", material->kAmbient);
	materialJSON->AddFloat("kDiffuse", material->kDiffuse);
	materialJSON->AddFloat("kSpecular", material->kSpecular);
	materialJSON->AddFloat("shininess", material->shininess);

	if (!material->GetTextures().empty())
	{
		JSON_value *texturesJSON = materialJSON->CreateValue(rapidjson::kArrayType);
		if (material->textures[(unsigned)TextureType::DIFFUSE] != nullptr)
		{
			texturesJSON->AddTexture("diffuse", material->textures[(unsigned)TextureType::DIFFUSE]);
		}
		if (material->textures[(unsigned)TextureType::SPECULAR] != nullptr)
		{
			texturesJSON->AddTexture("specular", material->textures[(unsigned)TextureType::SPECULAR]);
		}
		if (material->textures[(unsigned)TextureType::OCCLUSION] != nullptr)
		{
			texturesJSON->AddTexture("occlusion", material->textures[(unsigned)TextureType::OCCLUSION]);
		}
		if (material->textures[(unsigned)TextureType::EMISSIVE] != nullptr)
		{
			texturesJSON->AddTexture("emissive", material->textures[(unsigned)TextureType::EMISSIVE]);
		}
		materialJSON->AddValue("textures", texturesJSON);
	}

	if (material->shader != nullptr)
	{
		JSON_value *shaderJSON = materialJSON->CreateValue();
		shaderJSON->AddString("file", material->shader->file.c_str());
		shaderJSON->AddUint("value", material->shader->id);
	}
	json->AddValue("material", materialJSON);

	App->fsystem->Save((MATERIALS + material->name + JSONEXT).c_str(), json->ToString().c_str(),json->Size());
}
