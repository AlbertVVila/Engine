#include "ComponentImage.h"
#include "JSON.h"
#include "Imgui/imgui.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleUI.h"

#define None "None Selected"

ComponentImage::ComponentImage() : Component(nullptr, ComponentType::Image)
{
	//Refact
	if (textureFiles.size() == 0)
	{
		textureFiles = App->fsystem->ListFiles(TEXTURES, false);
	}
	App->ui->images.push_back(this);
}

ComponentImage::ComponentImage(GameObject* gameobject) : Component(gameobject, ComponentType::Image)
{
	if (textureFiles.size() == 0)
	{
		textureFiles = App->fsystem->ListFiles(TEXTURES, false);
	}
	App->ui->images.push_back(this);
}

ComponentImage::ComponentImage(const ComponentImage &copy) : Component(copy)
{
	color = copy.color;
	textureName = copy.textureName;
	App->ui->images.push_back(this);
}

ComponentImage::~ComponentImage()
{
	
}

Component * ComponentImage::Clone() const
{
	return new ComponentImage(*this);
}

void ComponentImage::DrawProperties()
{
	if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Enabled", &enabled);

		//texture selector
		if (ImGui::BeginCombo("Texture", textureName))
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
					texture = App->textures->GetTexture(textureName);
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
		ImGui::ColorEdit3("Font color", (float*)&color);

		ImGui::Separator();
	}
}

void ComponentImage::Save(JSON_value *value)const
{
	Component::Save(value);
	value->AddString("textureName", textureName);
	value->AddFloat3("color", color);
}
void ComponentImage::Load(const JSON_value &value)
{
	Component::Load(value);
	textureName = value.GetString("textureName");
	color = value.GetFloat3("color");
}