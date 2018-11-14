#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleTextures.h"

ComponentMaterial::ComponentMaterial(const aiMaterial * material) : Component(ComponentType::Material)
{
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
	std::string texturePath("Models/Baker/");
	texturePath += file.C_Str();
	////TODO: if texture was already loaded by another material, don't load it again
	texture = App->textures->Load(texturePath.c_str());
	//textures.push_back(texture);
}

Texture * ComponentMaterial::GetTexture() const
{
	return texture;
}
