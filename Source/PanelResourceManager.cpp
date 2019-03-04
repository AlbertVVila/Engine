#include "PanelResourceManager.h"

#include "Application.h"
#include "ModuleResourceManager.h"

#include "Resource.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"

#include "imgui.h"
#include <algorithm>
#define MAX_FILE 128
#define resourcePopup "Resource"

// Shorting functions
bool sortByUID(const Resource* a, const Resource* b) { return a->GetUID() > b->GetUID(); };
bool sortByFile(const Resource* a, const Resource* b) { return a->GetFile() > b->GetFile(); };
bool sortByExportedFile(const Resource* a, const Resource* b) { return a->GetExportedFile() > b->GetExportedFile(); };
bool sortByReference(const Resource* a, const Resource* b) { return a->GetReferences() > b->GetReferences(); };
bool sortByType(const Resource* a, const Resource* b) { return a->GetType() > b->GetType(); };

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
	if(auxResource == nullptr)
		UpdateResourcesList();

	ImGui::Columns(6);
	// Table references: UID | File | Exported File | References | Type |
	ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.0f, 1.0f, 1.0f));
	if (ImGui::Selectable("UID"))			{sortList = SORTING::UID;}			ImGui::SameLine(); ImGui::NextColumn();
	if (ImGui::Selectable("File"))			{sortList = SORTING::FILE;}			ImGui::SameLine(); ImGui::NextColumn();
	if (ImGui::Selectable("Exported File"))	{sortList = SORTING::EXPORTED;}		ImGui::SameLine(); ImGui::NextColumn();
	if (ImGui::Selectable("References"))	{sortList = SORTING::REFERENCES;}	ImGui::SameLine(); ImGui::NextColumn();
	if (ImGui::Selectable("Type"))			{sortList = SORTING::TYPE;}			ImGui::SameLine(); ImGui::NextColumn();
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "");						ImGui::NextColumn(); ImGui::Separator();
	ImGui::PopStyleColor(1);

	for each (auto resource in resourcesList)
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
			switch (resource->GetType())
			{
			case TYPE::TEXTURE: openTextureWindow = true; break;
			case TYPE::MESH: openMeshWindow = true; break;
			}
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
	if (openMeshWindow)
		DrawResourceMesh();
	ImGui::End();
}

void PanelResourceManager::UpdateResourcesList()
{
	resourcesList = App->resManager->GetResourcesList();

	switch (sortList)
	{
	default:
	case SORTING::NONE:
		break;
	case SORTING::UID:
		std::sort(resourcesList.begin(), resourcesList.end(), sortByUID);
		break;
	case SORTING::FILE:
		std::sort(resourcesList.begin(), resourcesList.end(), sortByFile);
		break;
	case SORTING::EXPORTED:
		std::sort(resourcesList.begin(), resourcesList.end(), sortByExportedFile);
		break;
	case SORTING::REFERENCES:
		std::sort(resourcesList.begin(), resourcesList.end(), sortByReference);
		break;
	case SORTING::TYPE:
		std::sort(resourcesList.begin(), resourcesList.end(), sortByType);
		break;
	}
}

void PanelResourceManager::OpenResourceEditor()
{
	if (openEditor)
	{
		if (!ImGui::IsPopupOpen(resourcePopup))
		{
			ImGui::OpenPopup(resourcePopup);
			auxResource = new Resource(*previous);
			// To avoid deleting the texture from memory using a fake reference
			auxReferences = auxResource->GetReferences();
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

void PanelResourceManager::DrawResourceMesh()
{
	if (!ImGui::Begin("Texture Manager", &openMeshWindow))
	{
		ImGui::End();
		return;
	}
	ResourceMesh& mesh = *(ResourceMesh*)previous;
	std::string exportedFile(mesh.GetExportedFile());
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), (exportedFile + ":").c_str());
	ImGui::Columns(2);
	ImGui::Text("VAO: %u", mesh.GetVAO());
	ImGui::Text("VBO: %u", mesh.GetVBO());
	ImGui::Text("EBO: %u", mesh.GetEBO());
	ImGui::Text("Number of Indices: %u", mesh.numIndices);
	ImGui::Text("Number of Vertices: %u", mesh.numVertices);
	ImGui::NextColumn();
	// TODO: [Resource Manager] Add preview of the mesh

	ImGui::End();
}

void PanelResourceManager::CleanUp()
{
	if(!openTextureWindow && !openMeshWindow)
		previous = nullptr;

	if (auxResource != nullptr)
		RELEASE(auxResource);

	openEditor = false;
}