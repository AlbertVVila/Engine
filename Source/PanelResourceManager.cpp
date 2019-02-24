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

		// Edit button
		if (ImGui::Button("Edit"))
		{
			openEditor = true;
			auxResource = resource;
		}
		ImGui::NextColumn();
		ImGui::PopID();
	}
	OpenResourceEditor();
	ImGui::End();
}

void PanelResourceManager::OpenResourceEditor()
{
	if (openEditor)
	{
		if (!ImGui::IsPopupOpen(resourcePopup))
		{
			ImGui::OpenPopup(resourcePopup);
			switch (auxResource->GetType())
			{
			case TYPE::TEXTURE:
				previous = new ResourceTexture(*(ResourceTexture*)auxResource);
				break;
			case TYPE::MESH:
				previous = new ResourceMesh((ResourceMesh&)auxResource);
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
			int loaded = auxResource->GetReferences();
			ImGui::InputInt("References", &loaded);
			auxResource->SetReferences(loaded);

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
				CleanUp();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				auxResource->Copy(*previous);
				CleanUp();
			}
			ImGui::EndPopup();
		}
	}
}

void PanelResourceManager::CleanUp()
{
	auxResource = nullptr;

	if (previous != nullptr)
		RELEASE(previous);

	openEditor = false;
}