#include "ComponentMaterial.h"
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
ComponentMaterial::ComponentMaterial(GameObject* gameobject, const char * material) : Component(gameobject, ComponentType::Material)
{
	this->shader = App->program->defaultProgram;
	SetMaterial(material);
}

ComponentMaterial::ComponentMaterial(const ComponentMaterial& component) : Component(component)
{
	texture = component.texture; //TODO: delete texture diferent materials?
	shader = component.shader;
	color = component.color;
	file = component.file;
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

void ComponentMaterial::SetMaterial(const char * material)
{
	if (material != nullptr)
	{
		texture = App->textures->Load(material);
	}
	//else //TODO: case CHECKERS vs color
	//{
	//	texture = App->textures->Load(CHECKERS);
	//	//texturePath = "checkersTexture.jpg";
	//}
	//shader = App->program->flatProgram;

	//TODO: if texture was already loaded by another material, don't load it again
	//DeleteTexture();
	//texture = App->textures->Load(texturePath.c_str());
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
		std::vector<std::string> shaders = App->fsystem->ListFiles(VERTEXSHADERS, false);
		if (ImGui::BeginCombo("Shader", selected_shader.c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < shaders.size(); n++)
			{
				bool is_selected = (selected_shader == shaders[n]);
				if (ImGui::Selectable(shaders[n].c_str(), is_selected) && selected_shader != shaders[n])
				{
					selected_shader = shaders[n];
					shader = App->program->GetProgram(selected_shader.c_str());
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		std::vector<std::string> textures = App->fsystem->ListFiles(TEXTURES, false);
		if (ImGui::BeginCombo("Textures", selected_texture.c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < textures.size(); n++)
			{
				bool is_selected = (selected_texture == textures[n]);
				if (ImGui::Selectable(textures[n].c_str(), is_selected) && selected_texture != textures[n])
				{
					selected_texture = textures[n];
					texture = App->textures->Load(selected_texture.c_str());
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
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
	if (!file.empty())
	{
		value->AddString("MaterialFile", file.c_str());
	}
}

void ComponentMaterial::Load(JSON_value * value)
{
	Component::Load(value);
	//TODO: deserialize shader
	color = value->GetFloat4("Color");
	const char *myfile = value->GetString("MaterialFile");
	if (myfile != nullptr)
	{
		file = myfile;
		SetMaterial(file.c_str());
	}
	//shader = App->program->flatProgram;
}
