#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

#include "GameObject.h"
#include "Imgui/imgui.h"
#include "GL/glew.h"

#include "assimp/material.h"
#include "JSON.h"
#include <vector>
ComponentMaterial::ComponentMaterial(GameObject* gameobject) : Component(gameobject, ComponentType::Renderer)
{
	shader = App->program->defaultShader;
}

ComponentMaterial::ComponentMaterial(const ComponentMaterial& component) : Component(component)
{
	
}

ComponentMaterial::~ComponentMaterial()
{
	DeleteTexture();
}

Component * ComponentMaterial::Clone() const
{
	return new ComponentMaterial(*this);
}

void ComponentMaterial::DeleteTexture()//TODO delete all textures of materials
{
	for (unsigned i = 0; i < MAXTEXTURES; i++)
	{
		if (textures[i] != nullptr)
		{
			glDeleteTextures(1, (GLuint*)&textures[i]->id);
			RELEASE(textures[i]);
		}
	}
}

void ComponentMaterial::SetTexture(const char * newTexture, TextureType type)
{
	if (newTexture != nullptr)
	{
		textures[(unsigned)type] = App->textures->GetTexture(newTexture);
	}
}

void ComponentMaterial::SetShader(const char * shaderName)
{
	if (shaderName != nullptr)
	{
		shader = App->program->GetProgram(shaderName); //TODO: refactor shader + texture + material
	}
}

Texture * ComponentMaterial::GetTexture(TextureType type) const
{
	return textures[(unsigned)type];
}

Shader * ComponentMaterial::GetShader() const
{
	return shader;
}

void ComponentMaterial::DrawProperties()
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
		std::vector<std::string> materials = App->fsystem->ListFiles(MATERIALS, false);
		if (ImGui::BeginCombo("Material", name.c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < materials.size(); n++)
			{
				bool is_selected = (name == materials[n]);
				if (ImGui::Selectable(materials[n].c_str(), is_selected) && name != materials[n])
				{
					name = materials[n];
					ComponentMaterial *mat = App->resManager->GetMaterial(name);
					if (mat == nullptr)
					{
						Load(name.c_str());
						App->resManager->AddMaterial(name, this);
					}
					else
					{
						//TODO: assign it?
					}
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		//std::vector<std::string> shaders = App->fsystem->ListFiles(VERTEXSHADERS, false);
		//if (ImGui::BeginCombo("Shader", selected_shader.c_str())) // The second parameter is the label previewed before opening the combo.
		//{
		//	for (int n = 0; n < shaders.size(); n++)
		//	{
		//		bool is_selected = (selected_shader == shaders[n]);
		//		if (ImGui::Selectable(shaders[n].c_str(), is_selected) && selected_shader != shaders[n])
		//		{
		//			selected_shader = shaders[n];
		//			shader = App->program->GetProgram(selected_shader.c_str());
		//		}
		//		if (is_selected)
		//			ImGui::SetItemDefaultFocus();
		//	}
		//	ImGui::EndCombo();
		//}
		/*ImGui::Text((name + " Material").c_str());
		std::vector<std::string> textureFiles = App->fsystem->ListFiles(TEXTURES, false);
		const char* textureTypes[] = { "Diffuse", "Specular", "Occlusion", "Emissive" };
		unsigned i = 0;
		for (auto &matTexture : textures)
		{
			ImGui::PushID(&matTexture);
			if (ImGui::BeginCombo(textureTypes[i], selected_texture[i].c_str())) 
			{
				for (int n = 0; n < textureFiles.size(); n++)
				{
					bool is_selected = (selected_texture[i] == textureFiles[n]);
					if (ImGui::Selectable(textureFiles[n].c_str(), is_selected) && selected_texture[i] != textureFiles[n])
					{
						selected_texture[i] = textureFiles[n];
						matTexture = App->textures->Load(selected_texture[i].c_str());
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			switch ((TextureType)i)
			{
				case TextureType::DIFFUSE:
					ImGui::ColorEdit4("Diffuse Color", (float*)&diffuse_color);
					break;
				case TextureType::SPECULAR:
					ImGui::ColorEdit3("Specular Color", (float*)&specular_color);
					break;
				case TextureType::EMISSIVE:
					ImGui::ColorEdit3("Emissive Color", (float*)&emissive_color);
					break;
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
		}*/
	}
	ImGui::PopID();
}

void ComponentMaterial::Save(JSON_value * value) const
{
	Component::Save(value);
	Save();
	value->AddString("Material", name.c_str());
}

void ComponentMaterial::Load(const JSON_value & value)
{
	Component::Load(value);
	const char* materialFile = value.GetString("Material");
	if (materialFile != nullptr)
	{
		Load(materialFile);
	}
}

void ComponentMaterial::Load(const char* materialfile)
{
	char* data = nullptr;
	std::string materialName(materialfile);
	App->fsystem->Load((MATERIALS + materialName + JSONEXT).c_str(), &data);
	JSON *json = new JSON(data);
	JSON_value *materialJSON = json->GetValue("material");

	name = materialName;
	diffuse_color = materialJSON->GetColor4("diffuseColor");
	specular_color = materialJSON->GetColor3("specularColor");
	emissive_color = materialJSON->GetColor3("emissiveColor");

	kAmbient = materialJSON->GetFloat("kAmbient");
	kDiffuse = materialJSON->GetFloat("kDiffuse");
	kSpecular = materialJSON->GetFloat("kSpecular");
	shininess = materialJSON->GetFloat("shininess");

	const char *diffuseFile = materialJSON->GetString("diffuse");
	if (diffuseFile != nullptr)
	{
		textures[(unsigned)TextureType::DIFFUSE] = App->textures->GetTexture(diffuseFile);
	}
	const char *specularFile = materialJSON->GetString("specular");
	if (specularFile != nullptr)
	{
		textures[(unsigned)TextureType::DIFFUSE] = App->textures->GetTexture(specularFile);
	}
	const char *occlusionFile = materialJSON->GetString("occlusion");
	if (occlusionFile != nullptr)
	{
		textures[(unsigned)TextureType::DIFFUSE] = App->textures->GetTexture(occlusionFile);
	}
	const char *emissiveFile = materialJSON->GetString("emissive");
	if (emissiveFile != nullptr)
	{
		textures[(unsigned)TextureType::DIFFUSE] = App->textures->GetTexture(emissiveFile);
	}

	const char* shaderName = materialJSON->GetString("shader");
	if (shaderName != nullptr)
	{
		shader = App->program->GetProgram(materialJSON->GetString("shader"));
	}
}

std::list<Texture*> ComponentMaterial::GetTextures() const
{
	std::list<Texture*> mytextures;
	for (unsigned i = 0; i < MAXTEXTURES; i++)
	{
		if (textures[i] != nullptr)
		{
			mytextures.push_back(textures[i]);
		}
	}
	return mytextures;
}

void ComponentMaterial::Save() const
{
	JSON *json = new JSON();
	JSON_value *materialJSON = json->CreateValue();

	if(textures[(unsigned) TextureType::DIFFUSE] != nullptr)
		materialJSON->AddFloat4("diffuseColor", diffuse_color);

	if (textures[(unsigned)TextureType::SPECULAR] != nullptr)
		materialJSON->AddFloat3("specularColor", specular_color);

	if (textures[(unsigned)TextureType::EMISSIVE] != nullptr)
		materialJSON->AddFloat3("emissiveColor", emissive_color);

	materialJSON->AddFloat("kAmbient", kAmbient);
	materialJSON->AddFloat("kDiffuse", kDiffuse);
	materialJSON->AddFloat("kSpecular", kSpecular);
	materialJSON->AddFloat("shininess", shininess);


	if (textures[(unsigned)TextureType::DIFFUSE] != nullptr)
	{
		materialJSON->AddString("diffuse", textures[(unsigned)TextureType::DIFFUSE]->file.c_str());
	}
	if (textures[(unsigned)TextureType::SPECULAR] != nullptr)
	{
		materialJSON->AddString("specular", textures[(unsigned)TextureType::SPECULAR]->file.c_str());
	}
	if (textures[(unsigned)TextureType::OCCLUSION] != nullptr)
	{
		materialJSON->AddString("occlusion", textures[(unsigned)TextureType::OCCLUSION]->file.c_str());
	}
	if (textures[(unsigned)TextureType::EMISSIVE] != nullptr)
	{
		materialJSON->AddString("emissive", textures[(unsigned)TextureType::EMISSIVE]->file.c_str());
	}

	if (shader != nullptr)
	{
		materialJSON->AddString("shader", shader->file.c_str());
	}

	json->AddValue("material", materialJSON);

	App->fsystem->Save((MATERIALS + name + JSONEXT).c_str(), json->ToString().c_str(),json->Size());
}
