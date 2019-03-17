#include "ComponentImage.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleUI.h"

#include "JSON.h"
#include "imgui.h"

#define None "None Selected"

ComponentImage::ComponentImage() : Component(nullptr, ComponentType::Image)
{
	//Refact
	if (textureFiles.size() == 0)
	{
		textureFiles = App->fsystem->ListFiles(TEXTURES, false);
	}
}

ComponentImage::ComponentImage(GameObject* gameobject) : Component(gameobject, ComponentType::Image)
{
	if (textureFiles.size() == 0)
	{
		textureFiles = App->fsystem->ListFiles(TEXTURES, false);
	}
}

ComponentImage::ComponentImage(const ComponentImage &copy) : Component(copy)
{
	color = copy.color;
	textureName = copy.textureName;
}

ComponentImage::~ComponentImage()
{
	App->resManager->DeleteTexture(textureName);
	texture = nullptr;
}

Component* ComponentImage::Clone() const
{
	return new ComponentImage(*this);
}

void ComponentImage::DrawProperties()
{
	if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool removed = Component::DrawComponentState();
		if (removed)
		{
			return;
		}

		//texture selector
		if (ImGui::BeginCombo("Texture", textureName.c_str()))
		{
			bool none_selected = (textureName == None);
			if (ImGui::Selectable(None, none_selected))
			{
				textureName = None;
				if (texture != nullptr)
				{
					App->resManager->DeleteTexture(textureName);
					texture = nullptr;
				}
			}
			if (none_selected)
				ImGui::SetItemDefaultFocus();
			for (int n = 0; n < textureFiles.size(); n++)
			{
				bool is_selected = (textureName == textureFiles[n]);
				if (ImGui::Selectable(textureFiles[n].c_str(), is_selected))
				{
					textureName = textureFiles[n].c_str();
					texture = App->textures->GetTexture(textureName.c_str());
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (texture != nullptr)
		{
			ImGui::Image((ImTextureID)texture->id, { 200,200 }, { 0,1 }, { 1,0 });
		}
		
		//color
		ImGui::ColorEdit4("Image color", (float*)&color);

		ImGui::Separator();
	}
}

void ComponentImage::Save(JSON_value *value)const
{
	Component::Save(value);
	value->AddString("textureName", textureName.c_str());
	value->AddFloat4("color", color);
}

void ComponentImage::Load(JSON_value* value)
{
	Component::Load(value);
	textureName = value->GetString("textureName");
	color = value->GetFloat4("color");	
	if (textureName != "None Selected")
	{
		texture = App->textures->GetTexture(textureName.c_str());
	}
}