#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleTextures.h"
#include "GameObject.h"
#include "Imgui/imgui.h"
#include "GL/glew.h"

ComponentMaterial::ComponentMaterial(GameObject* gameobject, const aiMaterial * material) : Component(gameobject, ComponentType::Material)
{
	this->shader = App->program->textureProgram;
	SetMaterial(material);
}

ComponentMaterial::~ComponentMaterial()
{
	DeleteTexture();
}

void ComponentMaterial::DeleteTexture()
{
	if (texture != nullptr)
	{
		glDeleteTextures(1, (GLuint*)&texture->id);
	}
	RELEASE(texture);
}

void ComponentMaterial::SetMaterial(const aiMaterial * material)
{
	std::string texturePath;
	if (material != nullptr)
	{
		aiTextureMapping mapping = aiTextureMapping_UV;
		aiString file;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, 0);
		texturePath =gameobject->GetFileFolder() + file.C_Str();
	}
	else
	{
		texturePath = "checkersTexture.jpg";
	}

	//TODO: if texture was already loaded by another material, don't load it again
	DeleteTexture();
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
		bool removed = Component::DrawComponentState();
		if (removed)
		{
			ImGui::PopID();
			return;
		}
		ImGui::Text("Shader: PlaceHolder");
		if (texture != nullptr)
		{
			ImGui::Text("Texture width:%d height:%d", texture->width, texture->height);
			float size = ImGui::GetWindowWidth();
			ImGui::Image((ImTextureID)texture->id, { size,size });
			ImGui::Separator();
		}
	}
	ImGui::PopID();
}
