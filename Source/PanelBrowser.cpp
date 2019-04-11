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
#define FOLDER_ICON "folderIconBlue.dds"
#define FILE_ICON "fileIconBlue.dds"
#define FBX_ICON "fbxIconBlue.dds"
#define PNG_ICON "pngIconBlue.dds"
#define JPG_ICON "jpgIconBlue.dds"
#define TGA_ICON "tgaIconBlue.dds"
#define TIF_ICON "tifIconBlue.dds"
#define DDS_ICON "ddsIconBlue.dds"
#define M4T_ICON "m4tIconBlue.dds"
#define JSON_ICON "jsonIconBlue.dds"
#define SC3NE_ICON "sc3neIconBlue.dds"

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
	sc3neIcon = (ResourceTexture*)App->resManager->Get(SC3NE_ICON);

	if (folderIcon == nullptr || fileIcon == nullptr || fbxIcon == nullptr || pngIcon == nullptr || jpgIcon == nullptr
		|| tgaIcon == nullptr || tifIcon == nullptr || ddsIcon == nullptr || m4tIcon == nullptr || jsonIcon == nullptr 
		|| sc3neIcon == nullptr)
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

	int i = 0;
	// If we are inside a folder show icon to go back
	if (path != ASSETS)
	{
		ImGui::SetCursorPosY(ICON_Y_MARGIN);
		if (ImGui::ImageButton(folderIcon != nullptr ? (ImTextureID)folderIcon->gpuID : 0, ImVec2(ICON_SIZE, ICON_SIZE), ImVec2(0, 1), ImVec2(1, 0), 1))
		{
			path = pathStack.top();
			pathStack.pop();
			folderContentDirty = true;
		}

		ImGui::SetCursorPosY(ICON_SIZE + ICON_Y_MARGIN);	
		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ICON_NAME_SIZE);
		ImGui::Text("..");
		if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			path = pathStack.top();
			pathStack.pop();
			folderContentDirty = true;
		}
		i++;
	}

	// Draw folder icons
	for each(std::string dir in dirs)
	{
		ImGui::PushID(dir.c_str());
		DrawFolderIcon(dir.c_str(), i);		
		i++;
		ImGui::PopID();
	}
	
	// Draw files icons
	for each (std::string file in files)
	{
		// Check that the file has been loaded on the Resource Manager
		if (App->resManager->FindByFileInAssets((path + file).c_str()) > 0)
		{
			DrawFileIcon(file.c_str(), i);
			i++;
		}
	}

	ImGui::EndChild();

	// Import Configuration Pop-up
	if (openImportConfigPopUp)
		DrawImportConfigurationPopUp();

	// Rename Pop-up
	if (openRenamePopUp)
		DrawRenamePopUp();

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
	ImGui::SetCursorPosY(ICON_Y_MARGIN + (ICON_SIZE + ICON_Y_MARGIN) * (itemNumber / maxNumberElements));

	if (ImGui::ImageButton(folderIcon != nullptr ? (ImTextureID)folderIcon->gpuID : 0, ImVec2(ICON_SIZE, ICON_SIZE), ImVec2(0, 1), ImVec2(1, 0), 1))
	{
		pathStack.push(path);
		path += dir;
		path += "/";
		folderContentDirty = true;
	}

	// Folder name text
	ImGui::SetCursorPosX(LEFT_INDENTATION + (ICON_SIZE + ICON_X_MARGIN) * (itemNumber % maxNumberElements));
	ImGui::SetCursorPosY((ICON_SIZE + ICON_Y_MARGIN) + (ICON_SIZE + ICON_Y_MARGIN) * (itemNumber / maxNumberElements));
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
	ImGui::SetCursorPosY(ICON_Y_MARGIN + (ICON_SIZE + ICON_Y_MARGIN) * (itemNumber / maxNumberElements));

	// Select icon by extension
	std::string extension = App->fsystem->GetExtension(file);
	if		(extension == PNG)			{ ImGui::ImageButton(pngIcon != nullptr ? (ImTextureID)pngIcon->gpuID : 0, ImVec2(40, 40), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == TIF)			{ ImGui::ImageButton(tifIcon != nullptr ? (ImTextureID)tifIcon->gpuID : 0, ImVec2(40, 40), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == JPG)			{ ImGui::ImageButton(jpgIcon != nullptr ? (ImTextureID)jpgIcon->gpuID : 0, ImVec2(40, 40), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == TGA)			{ ImGui::ImageButton(tgaIcon != nullptr ? (ImTextureID)tgaIcon->gpuID : 0, ImVec2(ICON_SIZE, ICON_SIZE), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == TEXTUREEXT)	{ ImGui::ImageButton(ddsIcon != nullptr ? (ImTextureID)ddsIcon->gpuID : 0, ImVec2(ICON_SIZE, ICON_SIZE), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == FBXEXTENSION)	{ ImGui::ImageButton(fbxIcon != nullptr ? (ImTextureID)fbxIcon->gpuID : 0, ImVec2(ICON_SIZE, ICON_SIZE), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == MATERIALEXT)	{ ImGui::ImageButton(m4tIcon != nullptr ? (ImTextureID)m4tIcon->gpuID : 0, ImVec2(ICON_SIZE, ICON_SIZE), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == SCENEEXTENSION){ ImGui::ImageButton(sc3neIcon != nullptr ? (ImTextureID)sc3neIcon->gpuID : 0, ImVec2(ICON_SIZE, ICON_SIZE), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else								{ ImGui::ImageButton(fileIcon != nullptr ? (ImTextureID)fileIcon->gpuID : 0, ImVec2(ICON_SIZE, ICON_SIZE), ImVec2(0, 1), ImVec2(1, 0), 1); }

	if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		unsigned selectedUID = App->resManager->FindByFileInAssets((path + file).c_str());	
		fileSelected = App->resManager->GetWithoutLoad(selectedUID);
		ImGui::OpenPopup("File Context Menu");
	}

	ImGui::SetCursorPosX(LEFT_INDENTATION + (ICON_SIZE + ICON_X_MARGIN) * (itemNumber % maxNumberElements));
	ImGui::SetCursorPosY((ICON_SIZE + ICON_Y_MARGIN) + (ICON_SIZE + ICON_Y_MARGIN) * (itemNumber / maxNumberElements));

	ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 52);
	ImGui::Text(file);

	if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		unsigned selectedUID = App->resManager->FindByFileInAssets((path + file).c_str());
		fileSelected = App->resManager->GetWithoutLoad(selectedUID);
		ImGui::OpenPopup("File Context Menu");
	}

	// Right click menu
	DrawFileContextMenu();
	ImGui::PopID();
}

void PanelBrowser::DrawFileContextMenu()
{
	if (ImGui::BeginPopup("File Context Menu"))
	{
		if (fileSelected == nullptr)
		{ 
			ImGui::EndPopup();
			return;
		}

		if (ImGui::Selectable("Rename"))
		{
			strcpy(newName, App->fsystem->GetFilename(fileSelected->GetExportedFile()).c_str());
			openRenamePopUp = true;
		}
		if(ImGui::Selectable("Delete"))
		{
			fileSelected->Delete();
			folderContentDirty = true;
		}
		if (ImGui::Selectable("Import Configuration"))
		{
			//Code to change import settings
			openImportConfigPopUp = true;
		}	
		ImGui::EndPopup();
	}
}

void PanelBrowser::DrawImportConfigurationPopUp()
{
	ImGui::OpenPopup("Import configuration");

	ImGui::SetNextWindowSizeConstraints(ImVec2(250.0f, 130.0f), ImVec2((float)App->window->width, (float)App->window->height));
	if (ImGui::BeginPopupModal("Import configuration", &openImportConfigPopUp))
	{
		ImGui::Text("%s", fileSelected->GetExportedFile());
		fileSelected->DrawImportConfiguration();

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

void PanelBrowser::DrawRenamePopUp()
{
	ImGui::OpenPopup("Rename File");

	ImGui::SetNextWindowSizeConstraints(ImVec2(270.0f, 150.0f), ImVec2((float)App->window->width, (float)App->window->height));
	if (ImGui::BeginPopupModal("Rename File", &openRenamePopUp))
	{
		ImGui::Text("New name:");
		ImGui::SameLine();

		ImGui::InputText("", newName, MAX_FILENAME);

		// Check if there isn't already a file with the same name
		invalidName = App->resManager->Exists(newName, fileSelected->GetType());

		if(invalidName)
			ImGui::Text("A file with that name already exists!");
		else
			ImGui::NewLine();

		ImGui::NewLine();
		if (ImGui::ButtonEx("Accept", ImVec2(0, 0), invalidName? ImGuiButtonFlags_Disabled : 0))
		{
			fileSelected->Rename(newName);
			folderContentDirty = true;
			strcpy(newName,"");
			openRenamePopUp = false;
		}
		ImGui::SameLine(0, 110);
		if (ImGui::Button("Cancel"))
		{
			strcpy(newName, "");
			openRenamePopUp = false;
		}
		ImGui::EndPopup();
	}
}