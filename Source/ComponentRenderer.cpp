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
		if (material->diffuseTexture != nullptr)
		{
			RELEASE(material->diffuseTexture);
			glDeleteTextures(1, (GLuint*)&material->diffuseTexture->id);
		}
		if (material->specularTexture != nullptr)
		{
			RELEASE(material->specularTexture);
			glDeleteTextures(1, (GLuint*)&material->specularTexture->id);
		}
		if (material->occlusionTexture != nullptr)
		{
			RELEASE(material->occlusionTexture);
			glDeleteTextures(1, (GLuint*)&material->occlusionTexture->id);
		}
		if (material->emissiveTexture != nullptr)
		{
			RELEASE(material->emissiveTexture);
			glDeleteTextures(1, (GLuint*)&material->emissiveTexture->id);
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
	switch (type)
	{
	case TextureType::DIFFUSE:
		return material->diffuseTexture;
		break;
	case TextureType::SPECULAR:
		return material->specularTexture;
		break;
	case TextureType::OCCLUSION:
		return material->occlusionTexture;
		break;
	case TextureType::EMISSIVE:
		return material->emissiveTexture;
		break;
	}
}

Shader * ComponentRenderer::GetShader() const
{
	return material->shader;
}

float4 ComponentRenderer::GetColor() const
{
	return material->color;
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
		std::vector<std::string> textures = App->fsystem->ListFiles(TEXTURES, false);
		std::list<Texture*> matTextures = material->GetTextures();
		const char* textureTypes[] = { "Diffuse", "Specular", "Occlusion", "Emissive" };
		unsigned i = 0;
		for (auto &matTexture : matTextures)
		{
			if (ImGui::BeginCombo(textureTypes[i], selected_texture.c_str())) 
			{
				for (int n = 0; n < textures.size(); n++)
				{
					bool is_selected = (selected_texture == textures[n]);
					if (ImGui::Selectable(textures[n].c_str(), is_selected) && selected_texture != textures[n])
					{
						selected_texture = textures[n];
						matTexture = App->textures->Load(selected_texture.c_str());
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
			++i;
		}
	}
	ImGui::PopID();
}

void ComponentRenderer::Save(JSON_value * value) const
{
	Component::Save(value);
	JSON_value* materialJSON = value->CreateValue(rapidjson::kArrayType);
	SaveMaterial(materialJSON);
	value->AddValue("Material", materialJSON);
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
	material->color = materialJSON->GetFloat4("color");

	material->kAmbient = materialJSON->GetFloat("kAmbient");
	material->kDiffuse = materialJSON->GetFloat("kDiffuse");
	material->kSpecular = materialJSON->GetFloat("kSpecular");
	material->shininess = materialJSON->GetFloat("shininess");

	JSON_value *textureJSON = materialJSON->GetValue("textures"); //TODO: check when no texture
	material->diffuseTexture = materialJSON->GetTexture("diffuse");
	material->specularTexture = materialJSON->GetTexture("specular");
	material->occlusionTexture = materialJSON->GetTexture("occlusion");
	material->emissiveTexture = materialJSON->GetTexture("emissive");

	JSON_value *shaderJSON = materialJSON->GetValue("shader");
	if (shaderJSON != nullptr)
	{
		material->shader = new Shader(shaderJSON->GetUint("value"), shaderJSON->GetString("file"));
	}
}

void ComponentRenderer::SaveMaterial(JSON_value * materialJSON) const
{
	materialJSON->AddFloat4("color", material->color);

	materialJSON->AddFloat("kAmbient", material->kAmbient);
	materialJSON->AddFloat("kDiffuse", material->kDiffuse);
	materialJSON->AddFloat("kSpecular", material->kSpecular);
	materialJSON->AddFloat("shininess", material->shininess);

	JSON_value *texturesJSON = materialJSON->CreateValue(rapidjson::kArrayType);
	if (material->diffuseTexture != nullptr)
	{
		texturesJSON->AddTexture("diffuse", material->diffuseTexture);
	}
	if (material->specularTexture != nullptr)
	{
		texturesJSON->AddTexture("specular", material->specularTexture);
	}
	if (material->occlusionTexture != nullptr)
	{
		texturesJSON->AddTexture("occlusion", material->occlusionTexture);
	}
	if (material->emissiveTexture != nullptr)
	{
		texturesJSON->AddTexture("emissive", material->emissiveTexture);
	}
	materialJSON->AddValue("textures", texturesJSON);

	if (material->shader != nullptr)
	{
		JSON_value *shaderJSON = materialJSON->CreateValue(rapidjson::kArrayType);
		shaderJSON->AddString("file", material->shader->file.c_str());
		shaderJSON->AddUint("value", material->shader->value);
	}
}
