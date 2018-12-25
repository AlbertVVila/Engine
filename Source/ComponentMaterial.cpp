#include "ComponentMaterial.h"
#include "Application.h"

#include "ModuleEditor.h"
#include "ModuleProgram.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

#include "GameObject.h"
#include "MaterialEditor.h"
#include "Imgui/imgui.h"
#include "GL/glew.h"

#include "assimp/material.h"
#include "JSON.h"
#include <vector>
ComponentMaterial::ComponentMaterial(GameObject* gameobject) : Component(gameobject, ComponentType::Renderer)
{
	SetMaterial(DEFAULTMAT);
}

ComponentMaterial::ComponentMaterial(const ComponentMaterial& component) : Component(component)
{
	//Clone + add 1 more usage
}

ComponentMaterial::~ComponentMaterial()
{
	DeleteTexture();
}

Component * ComponentMaterial::Clone() const
{
	return new ComponentMaterial(*this);
}

bool ComponentMaterial::CleanUp()
{
	if (material != nullptr)
	{
		return material->CleanUp();
	}
	return true;
}

void ComponentMaterial::DeleteTexture()//TODO delete all textures of materials
{
	for (unsigned i = 0; i < MAXTEXTURES; i++)
	{
		if (material->textures[i] != nullptr)
		{
			glDeleteTextures(1, (GLuint*)&material->textures[i]->id);
			RELEASE(material->textures[i]);
		}
	}
}

void ComponentMaterial::SetTexture(const char * newTexture, TextureType type)
{
	if (newTexture != nullptr)
	{
		material->textures[(unsigned)type] = App->textures->GetTexture(newTexture);
	}
}

void ComponentMaterial::SetShader(const char * shaderName)
{
	if (shaderName != nullptr)
	{
		material->shader = App->program->GetProgram(shaderName); //TODO: refactor shader + texture + material
	}
}

Texture * ComponentMaterial::GetTexture(TextureType type) const
{
	return material->textures[(unsigned)type];
}

Shader * ComponentMaterial::GetShader() const
{
	return material->shader;
}

void ComponentMaterial::DrawProperties()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool removed = Component::DrawComponentState();
		if (removed)
		{
			ImGui::PopID();
			return;
		}
		std::vector<std::string> materials = App->fsystem->ListFiles(MATERIALS, false);
		if (ImGui::BeginCombo("", material->name.c_str()))
		{
			for (int n = 0; n < materials.size(); n++)
			{
				bool is_selected = (material->name == materials[n]);
				if (ImGui::Selectable(materials[n].c_str(), is_selected) && material->name != materials[n])
				{
					SetMaterial(materials[n].c_str());
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		if (ImGui::Button("View"))
		{
			App->editor->materialEditor->material = material;
			App->editor->materialEditor->open = true; //materialpopup is only drawn once in module editor
			App->editor->materialEditor->isUsed = true; 
		}
	}
	ImGui::PopID();
}

void ComponentMaterial::Save(JSON_value * value) const
{
	Component::Save(value);
	material->Save();
	value->AddString("Material", material->name.c_str());
}

void ComponentMaterial::Load(const JSON_value & value)
{
	Component::Load(value);
	const char* materialFile = value.GetString("Material");
	if (materialFile != nullptr)
	{
		material->Load(materialFile);
	}
}

Material::~Material()
{
	if (shader != nullptr)
	{
		RELEASE(shader);
	}
	for (unsigned i = 0; i < MAXTEXTURES; i++)
	{
		if (textures[i] != nullptr)
		{
			RELEASE(textures[i]);
		}
	}
}

void Material::Load(const char* materialfile)
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



void Material::Save() const
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

bool Material::CleanUp()
{
	if (shader != nullptr)
	{
		App->resManager->DeleteProgram(shader->file);
	}
	for (unsigned i = 0; i < MAXTEXTURES; i++)
	{
		if (textures[i] != nullptr)
		{
			App->resManager->DeleteTexture(textures[i]->file);
		}
	}
	return true;
}

std::list<Texture*> ComponentMaterial::GetTextures() const
{
	std::list<Texture*> mytextures;
	for (unsigned i = 0; i < MAXTEXTURES; i++)
	{
		if (material->textures[i] != nullptr)
		{
			mytextures.push_back(material->textures[i]);
		}
	}
	return mytextures;
}

void ComponentMaterial::SetMaterial(const char *file)
{
	if (file != nullptr)
	{
		if (material != nullptr)
		{
			App->resManager->DeleteMaterial(material->name);
		}
		material = App->resManager->GetMaterial(file);
		if (material == nullptr)
		{
			material = new Material();
			material->Load(file);
			App->resManager->AddMaterial(material->name, material);
			return;
		}
		return;
	}
	if (material != nullptr)
	{
		material = new Material();
		material->Load(DEFAULTMAT);
	}
}

void ComponentMaterial::SetMaterial(const Material &mat) //TODO: add nbusages to resmanager
{
	material->name = mat.name;

	material->diffuse_color = mat.diffuse_color;
	material->specular_color = mat.specular_color;
	material->emissive_color = mat.emissive_color;

	material->kAmbient = mat.kAmbient;
	material->kDiffuse = mat.kDiffuse;
	material->kSpecular = mat.kSpecular;
	material->shininess = mat.shininess;

	material->shader = mat.shader;
	for (unsigned i = 0; i < MAXTEXTURES; i++)
	{
		material->textures[i] = mat.textures[i];
	}
}

