#include "PanelResourceManager.h"

#include "Application.h"
#include "ModuleResourceManager.h"

#include "Resource.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"

#include "imgui.h"
#define MAX_FILE 128
#define resourcePopup "Resource"

PanelResourceManager::PanelResourceManager()
{
	enabled = false;
}

PanelResourceManager::~PanelResourceManager()
{
}

void PanelResourceManager::Draw()
{
	if (!ImGui::Begin("Resource Manager", &enabled))
	{
		ImGui::End();
		return;
	}

	ImGui::Columns(6);
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "UID"); ImGui::SameLine(); ImGui::NextColumn();
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "File"); ImGui::SameLine(); ImGui::NextColumn();
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Exported File"); ImGui::SameLine(); ImGui::NextColumn();
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "References"); ImGui::SameLine(); ImGui::NextColumn();
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Type"); ImGui::SameLine(); ImGui::NextColumn();
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), ""); ImGui::NextColumn(); ImGui::Separator();

	for each (auto resource in App->resManager->GetResourcesList())
	{
		unsigned uid = resource->GetUID();
		ImGui::PushID(uid);
		// UID
		ImGui::Text("%u", uid);
		ImGui::NextColumn();

		// File
		ImGui::Text(resource->GetFile());
		ImGui::NextColumn();

		// Exported File
		ImGui::Text(resource->GetExportedFile());
		ImGui::NextColumn();

		// Loaded
		ImGui::Text("%u", resource->GetReferences());
		ImGui::NextColumn();

		// Type
		switch (resource->GetType())
		{
		case TYPE::TEXTURE:
			ImGui::Text("Texture");
			break;
		case TYPE::MESH:
			ImGui::Text("Mesh");
			break;
		case TYPE::AUDIO:
			ImGui::Text("Audio");
			break;
		case TYPE::SCENE:
			ImGui::Text("Scene");
			break;
		case TYPE::BONE:
			ImGui::Text("Bone");
			break;
		case TYPE::ANIMATION:
			ImGui::Text("Animation");
			break;
		default:
		case TYPE::UNKNOWN:
			ImGui::Text("Unknown");
			break;
		}
		ImGui::NextColumn();
		// View button
		if (ImGui::Button("View"))
		{
			openTextureWindow = true;
			previous = resource;
		}
		ImGui::SameLine();
		// Edit button
		if (ImGui::Button("Edit"))
		{
			openEditor = true;
			previous = resource;
		}
		ImGui::NextColumn();
		ImGui::PopID();
	}
	OpenResourceEditor();
	if (openTextureWindow)
		DrawResourceTexture();
	ImGui::End();
}

void PanelResourceManager::OpenResourceEditor()
{
	if (openEditor)
	{
		if (!ImGui::IsPopupOpen(resourcePopup))
		{
			ImGui::OpenPopup(resourcePopup);
			switch (previous->GetType())
			{
			case TYPE::TEXTURE:
				auxResource = new ResourceTexture(*(ResourceTexture*)previous);
				// To avoid deleting the texture from memory using a fake ID and reference
				auxReferences = auxResource->GetReferences();
				((ResourceTexture*)auxResource)->gpuID = 0u;	
				break;
			case TYPE::MESH:
				auxResource = new ResourceMesh(*(ResourceMesh*)previous);
				break;
			case TYPE::AUDIO:
				ImGui::Text("Audio");
				break;
			case TYPE::SCENE:
				ImGui::Text("Scene");
				break;
			case TYPE::BONE:
				ImGui::Text("Bone");
				break;
			case TYPE::ANIMATION:
				ImGui::Text("Animation");
				break;
			default:
			case TYPE::UNKNOWN:
				ImGui::Text("Unknown");
				break;
			}
		}
		ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(500, 500));
		if (ImGui::BeginPopupModal(resourcePopup, NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			// UID
			unsigned uid = auxResource->GetUID();
			ImGui::Text("UID: %u", uid);

			// File
			char file[64] = "";
			strcpy(file, auxResource->GetFile());
			ImGui::InputText("File", file, MAX_FILE);
			auxResource->SetFile(file);

			// Exported File
			char exportedFile[64] = "";
			strcpy(exportedFile, auxResource->GetExportedFile());
			ImGui::InputText("Exported File", exportedFile, MAX_FILE);
			auxResource->SetExportedFile(exportedFile);

			// Loaded
			if (ImGui::InputInt("References", &auxReferences))
			{
				if (auxReferences < 0)
					auxReferences = 0;
			}

			// Type
			const char * types[] = { "Texture", "Mesh", "Audio", "Scene", "Bone", "Animation", "Unknown" };
			int type = (int)auxResource->GetType();
			if (ImGui::BeginCombo("Type", types[type]))
			{
				for (int n = 0; n < (int)TYPE::UNKNOWN; n++)
				{
					bool is_selected = (type == n);
					if (ImGui::Selectable(types[n], is_selected) && type != n)
					{
						auxResource->SetType((TYPE)n);
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			if (ImGui::Button("Apply"))
			{
				auxResource->SetReferences(auxReferences);
				previous->Resource::Copy(*auxResource);
				CleanUp();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				CleanUp();
			}
			ImGui::EndPopup();
		}
	}
}

void PanelResourceManager::DrawResourceTexture()
{
	if (!ImGui::Begin("Texture Manager", &openTextureWindow))
	{
		ImGui::End();
		return;
	}
	ResourceTexture& texture = *(ResourceTexture*)previous;
	std::string exportedFile(texture.GetExportedFile());
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), (exportedFile +":").c_str());
	ImGui::Columns(2);
	ImGui::Text("Width: %u", texture.width);
	ImGui::Text("Height: %u", texture.height);
	ImGui::Text("Depth: %u", texture.depth);
	ImGui::Text("Mips: %u", texture.mips);
	ImGui::Text("Bytes: %u", texture.bytes);
	ImGui::Text("GPU ID: %u", texture.gpuID);
	ImGui::Text("Format: %u", texture.format);
	ImGui::NextColumn();
	ImGui::Image((ImTextureID)texture.gpuID, ImVec2(160.0f, 160.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
	
	ImGui::End();
}

void PanelResourceManager::CleanUp()
{
	if(!openTextureWindow)
		previous = nullptr;

	if (auxResource != nullptr)
		RELEASE(auxResource);

	openEditor = false;
}