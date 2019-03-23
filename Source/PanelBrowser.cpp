#include "PanelBrowser.h"
#include "Globals.h"
#include "SDL.h"
#include "IL/il.h"
#include "assimp/version.h"
#include "GL/glew.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "rapidjson/rapidjson.h"
#include "physfs.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "GameObject.h"

// Icons
#define FOLDER_ICON "folderIconBlue"
#define FILE_ICON "fileIconBlue"
#define FBX_ICON "fbxIconBlue"
#define PNG_ICON "pngIconBlue"
#define JPG_ICON "jpgIconBlue"
#define TGA_ICON "tgaIconBlue"
#define TIF_ICON "tifIconBlue"
#define DDS_ICON "ddsIconBlue"
#define M4T_ICON "m4tIconBlue"
#define JSON_ICON "jsonIconBlue"

// ImGui elements sizes:
#define WINDOW_LOW_MARGIN 60
#define ICON_SIZE 40
#define ICON_X_MARGIN 20
#define ICON_Y_MARGIN 32
#define LEFT_INDENTATION 15
#define ICON_NAME_SIZE ICON_SIZE + 12

// |<--X Indentation--><--Icon Size--><--Icon X Margin--><--Next Icon---> _
// |				   _______________					 _______________  |
// |				  |				  |					|				| |
// |				  |				  |					|				| |
// |				  |				  |					|				| |
// |				  |				  |					|				| Icon Size
// |				  |				  |					|				| |
// |				  |				  |					|				| |
// |				  |_______________|					|_______________| _
// |					  IconName							IconName	  |
// |				  <--IconNameSize->					<--IconNameSize-> Icon Y Margin
// |																	  | 
// |				   _______________					 _______________  _
// |				  |				  |					|				| |
// |				  |				  |					|				| |
// |				  |				  |					|				| |
// |				  |				  |					|				| Icon Size
// |				  |				  |					|				| |
// |				  |				  |					|				| |
// |				  |_______________|					|_______________| _
// |					  IconName							IconName	  |
// |																	  |
// |																	  Window Low Margin
// |																	  |
// |______________________________________________________________________|

PanelBrowser::PanelBrowser() : path(ASSETS)
{
}

PanelBrowser::~PanelBrowser() 
{
	/*App->resManager->DeleteResource(folderIcon->GetUID());
	App->resManager->DeleteResource(fileIcon->GetUID());
	App->resManager->DeleteResource(fbxIcon->GetUID());
	App->resManager->DeleteResource(pngIcon->GetUID());
	App->resManager->DeleteResource(jpgIcon->GetUID());
	App->resManager->DeleteResource(tgaIcon->GetUID());
	App->resManager->DeleteResource(ddsIcon->GetUID());
	App->resManager->DeleteResource(m4tIcon->GetUID());
	App->resManager->DeleteResource(jsonIcon->GetUID());*/

	while (!pathStack.empty())
		pathStack.pop();

	files.clear();
	dirs.clear();

	fileSelected = nullptr;
}

bool PanelBrowser::Init()
{
	folderIcon = (ResourceTexture*)App->resManager->Get(FOLDER_ICON);
	fileIcon = (ResourceTexture*)App->resManager->Get(FILE_ICON);
	fbxIcon = (ResourceTexture*)App->resManager->Get(FBX_ICON);
	pngIcon = (ResourceTexture*)App->resManager->Get(PNG_ICON);
	jpgIcon = (ResourceTexture*)App->resManager->Get(JPG_ICON);
	tgaIcon = (ResourceTexture*)App->resManager->Get(TGA_ICON);
	tifIcon = (ResourceTexture*)App->resManager->Get(TIF_ICON);
	ddsIcon = (ResourceTexture*)App->resManager->Get(DDS_ICON);
	m4tIcon = (ResourceTexture*)App->resManager->Get(M4T_ICON);
	jsonIcon = (ResourceTexture*)App->resManager->Get(JSON_ICON);

	if (folderIcon == nullptr || fileIcon == nullptr || fbxIcon == nullptr || pngIcon == nullptr || jpgIcon == nullptr
		|| tgaIcon == nullptr || tifIcon == nullptr || ddsIcon == nullptr || m4tIcon == nullptr || jsonIcon == nullptr)
	{
		LOG("Warning: Some icons missing");
		return false;
	}

	return true;
}

void PanelBrowser::Draw() 
{
	if (!ImGui::Begin("Assets", &enabled))
	{
		ImGui::End();
		return;
	}

	// Get list of all files and directories when changes are detected
	if (folderContentDirty)
	{
		files.clear();
		dirs.clear();
		App->fsystem->ListFolderContent(path.c_str(), files, dirs);
		folderContentDirty = false;
	}

	// Draw current path
	ImGui::Text(path.c_str());

	// Icons area
	ImGui::BeginChild("Files", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowContentRegionMax().y - WINDOW_LOW_MARGIN), true, ImGuiWindowFlags_HorizontalScrollbar);

	// If we are inside a folder show icon to go back
	if (path != ASSETS)
	{
		if (ImGui::ImageButton(folderIcon != nullptr ? (ImTextureID)folderIcon->gpuID : 0, ImVec2(ICON_SIZE, ICON_SIZE), ImVec2(0, 1), ImVec2(1, 0), 1))
		{
			path = pathStack.top();
			pathStack.pop();
			folderContentDirty = true;
		}

		ImGui::SetCursorPosY(ICON_SIZE + ICON_Y_MARGIN - 20);	
		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ICON_NAME_SIZE);
		ImGui::Text("..");
		if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			path = pathStack.top();
			pathStack.pop();
			folderContentDirty = true;
		}
	}

	// Draw folder icons
	int i = 0;
	for each(std::string dir in dirs)
	{
		ImGui::PushID(dir.c_str());
		DrawFolderIcon(dir.c_str(), i);		
		i++;
		ImGui::PopID();
	}
	
	// Draw files icons
	int j = 0;
	for each (std::string file in files)
	{
		// Check that the file has been loaded on the Resource Manager
		if (App->resManager->FindByFileInAssets((path + file).c_str()) > 0)
		{
			DrawFileIcon(file.c_str(), j);
			j++;
		}
	}

	ImGui::EndChild();

	// Import Configuration Pop-up
	if (openImportConfigPopUp)
		DrawImportConfigurationPopUp();

	ImGui::End();	
}

void PanelBrowser::DrawFolderIcon(const char* dir, int itemNumber)
{
	ImGuiContext* context = ImGui::GetCurrentContext();
	ImVec2 size = context->CurrentWindow->Size;

	// Calculate number of items per row
	int maxNumberElements = size.x / (ICON_SIZE + ICON_X_MARGIN);
	if (maxNumberElements < 1) maxNumberElements = 1;

	// Folder icon position
	ImGui::SetCursorPosX(LEFT_INDENTATION + (ICON_SIZE + ICON_X_MARGIN) * (itemNumber % maxNumberElements));
	ImGui::SetCursorPosY((ICON_SIZE + ICON_Y_MARGIN) + (ICON_SIZE + ICON_Y_MARGIN) * (itemNumber / maxNumberElements));

	if (ImGui::ImageButton(folderIcon != nullptr ? (ImTextureID)folderIcon->gpuID : 0, ImVec2(ICON_SIZE, ICON_SIZE), ImVec2(0, 1), ImVec2(1, 0), 1))
	{
		pathStack.push(path);
		path += dir;
		path += "/";
		folderContentDirty = true;
	}

	// Folder name text
	ImGui::SetCursorPosX(LEFT_INDENTATION + (ICON_SIZE + ICON_X_MARGIN) * (itemNumber % maxNumberElements));
	ImGui::SetCursorPosY(ICON_SIZE + (ICON_SIZE + ICON_Y_MARGIN) + (ICON_SIZE + ICON_Y_MARGIN) * (itemNumber / maxNumberElements));
	ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ICON_NAME_SIZE);

	ImGui::Text(dir);
	if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		pathStack.push(path);
		path += dir;
		path += "/";
		folderContentDirty = true;
	}
}

void PanelBrowser::DrawFileIcon(const char* file, int itemNumber)
{
	ImGui::PushID(file);
	ImGuiContext* context = ImGui::GetCurrentContext();
	ImVec2 size = context->CurrentWindow->Size;

	// Calculate number of items per row
	int maxNumberElements = size.x / (ICON_SIZE + ICON_X_MARGIN);
	if (maxNumberElements < 1) maxNumberElements = 1;

	// File icon position
	ImGui::SetCursorPosX(LEFT_INDENTATION + (ICON_SIZE + ICON_X_MARGIN) * (itemNumber % maxNumberElements));
	ImGui::SetCursorPosY((ICON_SIZE + ICON_Y_MARGIN) + (ICON_SIZE + ICON_Y_MARGIN) * (itemNumber / maxNumberElements));

	// Select icon by extension
	std::string extension = App->fsystem->GetExtension(file);
	if		(extension == PNG)			{ ImGui::ImageButton(pngIcon != nullptr ? (ImTextureID)pngIcon->gpuID : 0, ImVec2(40, 40), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == TIF)			{ ImGui::ImageButton(tifIcon != nullptr ? (ImTextureID)tifIcon->gpuID : 0, ImVec2(40, 40), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == JPG)			{ ImGui::ImageButton(jpgIcon != nullptr ? (ImTextureID)jpgIcon->gpuID : 0, ImVec2(40, 40), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == TGA)			{ ImGui::ImageButton(tgaIcon != nullptr ? (ImTextureID)tgaIcon->gpuID : 0, ImVec2(ICON_SIZE, ICON_SIZE), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == TEXTUREEXT)	{ ImGui::ImageButton(ddsIcon != nullptr ? (ImTextureID)ddsIcon->gpuID : 0, ImVec2(ICON_SIZE, ICON_SIZE), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == FBXEXTENSION)	{ ImGui::ImageButton(fbxIcon != nullptr ? (ImTextureID)fbxIcon->gpuID : 0, ImVec2(ICON_SIZE, ICON_SIZE), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == MATERIALEXT)	{ ImGui::ImageButton(m4tIcon != nullptr ? (ImTextureID)m4tIcon->gpuID : 0, ImVec2(ICON_SIZE, ICON_SIZE), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == JSONEXT)		{ ImGui::ImageButton(jsonIcon != nullptr ? (ImTextureID)jsonIcon->gpuID : 0, ImVec2(ICON_SIZE, ICON_SIZE), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else								{ ImGui::ImageButton(fileIcon != nullptr ? (ImTextureID)fileIcon->gpuID : 0, ImVec2(ICON_SIZE, ICON_SIZE), ImVec2(0, 1), ImVec2(1, 0), 1); }

	if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		//fileSelected = path + file;
		fileSelected = App->resManager->GetWithoutLoad(App->fsystem->GetFilename(file).c_str());
		ImGui::OpenPopup("File Context Menu");
	}

	ImGui::SetCursorPosX(LEFT_INDENTATION + (ICON_SIZE + ICON_X_MARGIN) * (itemNumber % maxNumberElements));
	ImGui::SetCursorPosY(ICON_SIZE + (ICON_SIZE + ICON_Y_MARGIN) + (ICON_SIZE + ICON_Y_MARGIN) * (itemNumber / maxNumberElements));

	ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 52);
	ImGui::Text(file);

	if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		//fileSelected = path + file;
		fileSelected = App->resManager->GetWithoutLoad(App->fsystem->GetFilename(file).c_str());
		ImGui::OpenPopup("File Context Menu");
	}

	// Right click menu
	DrawFileContextMenu();
	ImGui::PopID();
}

void PanelBrowser::DrawImportConfigurationPopUp()
{
	ImGui::OpenPopup("Import configuration");

	if (ImGui::BeginPopupModal("Import configuration", &openImportConfigPopUp))
	{

		ImGui::Text("%s", fileSelected->GetExportedFile());
		switch (fileSelected->GetType())
		{
		case TYPE::TEXTURE:
			App->textures->DrawImportConfiguration((ResourceTexture*)fileSelected);
			break;
		}

		if (ImGui::Button("Accept"))
		{
			// Add accept logic
			openImportConfigPopUp = false;
			App->resManager->ReImportFile(fileSelected, path.c_str(), fileSelected->GetType());
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			openImportConfigPopUp = false;
		}
		ImGui::EndPopup();
	}
}

void PanelBrowser::DrawFileContextMenu()
{
	if (ImGui::BeginPopup("File Context Menu"))
	{
		ImGui::Text("Rename");
		if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			ImGui::OpenPopup("Rename File");
			//ImGui::EndPopup();
		}
		ImGui::Text("Delete");
		if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			App->fsystem->Remove(fileSelected->GetExportedFile());
			//ImGui::EndPopup();
		}
		if (ImGui::Selectable("Import Configuration"))
		{
			//Code to change import settings
			//ImGui::EndPopup();
			openImportConfigPopUp = true;
		}

		if (ImGui::BeginPopup("Rename File"))
		{
			// TODO: Add function for rename and rename file also on Resource Manager 
			ImGui::Text("Write the new name for the file");
			char* newName = new char[30];
			memset(newName, 0, 30);
			size_t pos1 = 0;
			size_t pos2 = 0;
			std::vector<std::string> result;
			char separator = '/';
			/*while (pos2 != fileSelected.npos)
			{
				pos2 = fileSelected.find(separator, pos1);
				if (pos2 != fileSelected.npos)
				{
					if (pos2 > pos1)
						result.push_back(fileSelected.substr(pos1, pos2 - pos1));
					pos1 = pos2 + 1;
				}
			}
			result.push_back(fileSelected.substr(pos1, fileSelected.size() - pos1));
			std::string filename = result.back();
			std::string auxRoute = fileSelected;
			std::string ruteToFile = auxRoute.replace(fileSelected.size() - filename.size() - 1, fileSelected.size() - 1, "/");
			ImGui::InputText(newName, newName, (size_t)30);
			ImGui::Button("Rename it");
			if (newName != "" && ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				App->fsystem->Rename(ruteToFile.c_str(), filename.c_str(), newName);
				ImGui::EndPopup();
			}*/
			ImGui::EndPopup();
		}
		ImGui::EndPopup();
	}
}