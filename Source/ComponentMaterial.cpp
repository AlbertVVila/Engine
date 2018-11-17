#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleTextures.h"
#include "GameObject.h"
#include "Imgui/imgui.h"

ComponentMaterial::ComponentMaterial(GameObject* gameobject, const aiMaterial * material) : Component(gameobject, ComponentType::Material)
{
	this->shader = App->program->textureProgram;
	if (material != nullptr)
	{
		SetMaterial(material);
	}
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::SetMaterial(const aiMaterial * material)
{
	aiTextureMapping mapping = aiTextureMapping_UV;
	aiString file;
	material->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, 0);
	//TODO: Relative path 
	std::string texturePath(gameobject->GetFileFolder());
	texturePath += file.C_Str();
	//TODO: if texture was already loaded by another material, don't load it again
	texture = App->textures->Load(texturePath.c_str());
	//textures.push_back(texture);
}

Texture * ComponentMaterial::GetTexture() const
{
	return texture;
}

unsigned int ComponentMaterial::GetShader()
{
	return shader;
}

void ComponentMaterial::DrawProperties()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Material"))
	{
		ImGui::Checkbox("Active", &enabled);
		ImGui::Text("Shader: PlaceHolder");
		ImGui::Text("Texture width:%d height:%d", texture->width, texture->height);
		float size = ImGui::GetWindowWidth();
		ImGui::Image((ImTextureID)texture->id, { size,size });
		ImGui::Separator();
	}
	ImGui::PopID();
}
