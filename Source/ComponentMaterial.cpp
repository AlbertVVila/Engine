#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleTextures.h"
#include "GameObject.h"
#include "Imgui/imgui.h"
#include "GL/glew.h"

#include "assimp/material.h"
#include "JSON.h"
ComponentMaterial::ComponentMaterial(GameObject* gameobject, const aiMaterial * material) : Component(gameobject, ComponentType::Material)
{
	this->shader = App->program->textureProgram;
	SetMaterial(material);
}

ComponentMaterial::ComponentMaterial(const ComponentMaterial& component) : Component(component)
{
	texture = component.texture; //TODO: delete texture diferent materials?
	shader = component.shader;
}

ComponentMaterial::~ComponentMaterial()
{
	DeleteTexture();
}

Component * ComponentMaterial::Clone()
{
	return new ComponentMaterial(*this);
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

unsigned int ComponentMaterial::GetShader() const
{
	return shader;
}

float4 ComponentMaterial::GetColor() const
{
	return color;
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

void ComponentMaterial::Save(JSON_value * value) const
{
	Component::Save(value);
	//TODO: serialize shader
	value->AddFloat4("Color", color);
	if (file != nullptr)
	{
		value->AddString("MaterialFile", file);
	}
}

void ComponentMaterial::Load(JSON_value * value)
{
	Component::Load(value);
	//TODO: deserialize shader
	color = value->GetFloat4("Color");
	file = value->GetString("MaterialFile");
}
