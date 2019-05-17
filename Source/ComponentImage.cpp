#include "ComponentImage.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleUI.h"

#include "ResourceTexture.h"

#include "HashString.h"
#include "JSON.h"
#include "imgui.h"

#define None "None Selected"

ComponentImage::ComponentImage() : Component(nullptr, ComponentType::Image)
{
	//Refact
	if (textureFiles.size() == 0)
	{
		UpdateTexturesList();
	}
}

ComponentImage::ComponentImage(GameObject* gameobject) : Component(gameobject, ComponentType::Image)
{
	if (textureFiles.size() == 0)
	{
		UpdateTexturesList();
	}
}

ComponentImage::ComponentImage(const ComponentImage &copy) : Component(copy)
{
	if (textureFiles.size() == 0)
	{
		UpdateTexturesList();
	}
	color = copy.color;
	flipHorizontal = copy.flipHorizontal;
	flipVertical = copy.flipVertical;
	texture = (ResourceTexture*)App->resManager->Get(copy.texture->GetUID());
}

ComponentImage::~ComponentImage()
{
	if (texture != nullptr) 
	{
		App->resManager->DeleteResource(texture->GetUID());
		texture = nullptr;
	}
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
		if (ImGui::BeginCombo("Texture", texture != nullptr ? texture->GetName() : None))
		{
			bool none_selected = (texture == nullptr);
			if (ImGui::Selectable(None, none_selected))
			{
				if (texture != nullptr)
				{
					App->resManager->DeleteResource(texture->GetUID());
					texture = nullptr;
				}
			}
			if (none_selected)
				ImGui::SetItemDefaultFocus();
			for (int n = 0; n < textureFiles.size(); n++)
			{
				bool is_selected = (texture != nullptr && (HashString(texture->GetName()) == HashString(textureFiles[n].c_str())));
				if (ImGui::Selectable(textureFiles[n].c_str(), is_selected) && !is_selected)
				{
					// Delete previous texture
					if (texture != nullptr)
						App->resManager->DeleteResource(texture->GetUID());

					texture = (ResourceTexture*)App->resManager->GetByName(textureFiles[n].c_str(), TYPE::TEXTURE);
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Refresh List"))
		{
			UpdateTexturesList();
		}
		if (texture != nullptr)
		{
			ImGui::Image((ImTextureID)texture->gpuID, { 200,200 }, { 0,1 }, { 1,0 });
		}
		
		//color
		ImGui::ColorEdit4("Image color", (float*)&color);

		ImGui::Checkbox("Flip Vertical", &flipVertical);
		ImGui::Checkbox("Flip Horizontal", &flipHorizontal);

		ImGui::Separator();
	}
}

void ComponentImage::UpdateTexturesList()
{
	textureFiles.clear();
	textureFiles = App->resManager->GetResourceNamesList(TYPE::TEXTURE, true);
}

void ComponentImage::Save(JSON_value *value)const
{
	Component::Save(value);
	value->AddUint("textureUID", texture->GetUID());
	value->AddFloat4("color", color);
	value->AddInt("FlipVertical", flipVertical);
	value->AddInt("FlipHorizontal", flipHorizontal);
}

void ComponentImage::Load(JSON_value* value)
{
	Component::Load(value);
	unsigned uid = value->GetUint("textureUID");
	texture = (ResourceTexture*)App->resManager->Get(uid);
	color = value->GetFloat4("color");	
	flipVertical = value->GetInt("FlipVertical");
	flipHorizontal = value->GetInt("FlipHorizontal");
}