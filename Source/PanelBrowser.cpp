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

		
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(.0f, 0.5f));
	
	ImGui::Text(path.c_str());

	// If we are inside a folder show icon to go back
	if(path != ASSETS)
	{
		ImGui::ImageButton(folderIcon != nullptr? (ImTextureID)folderIcon->gpuID : 0, ImVec2(40, 40), ImVec2(0, 1), ImVec2(1, 0), 1);
		if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			path = pathStack.top();
			pathStack.pop();
			folderContentDirty = true;
		}
		
		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 52);
		ImGui::Text("..");
		if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			path = pathStack.top();
			pathStack.pop();
			folderContentDirty = true;
		}
		ImGui::SetCursorPosX(15);
		ImGui::SetCursorPosY(10);
	}

	// Draw folder icons
	int i = 0;
	for each(std::string dir in dirs)
	{
		DrawFolderIcon(dir.c_str(), i);		
		i++;
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

	// Right click menu
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

	// Import Configuration Pop-up
	if (openImportConfigPopUp)
		DrawImportConfigurationPopUp();

	ImGui::PopStyleVar();
	ImGui::End();	
}

void PanelBrowser::DrawFolderIcon(const char* dir, int itemNumber)
{
	ImGuiContext* context = ImGui::GetCurrentContext();
	ImVec2 size = context->CurrentWindow->Size;
	int maxNumberElements = size.x / 60;
	if (maxNumberElements < 1) maxNumberElements = 1;

	ImGui::SetCursorPosX(15 + 60 * (itemNumber % maxNumberElements));
	ImGui::SetCursorPosY(120 + 72 * (itemNumber / maxNumberElements));

	ImGui::ImageButton(folderIcon != nullptr ? (ImTextureID)folderIcon->gpuID : 0, ImVec2(iconSize.x, iconSize.y), ImVec2(0, 1), ImVec2(1, 0), 1);

	if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		pathStack.push(path);
		path += dir;
		path += "/";
		folderContentDirty = true;
	}

	float2 nextWinSize = float2(350, 100);
	ImGui::SetNextWindowPos(ImVec2((App->window->width / 2) - nextWinSize.x / 2, (App->window->height / 2) - nextWinSize.y / 2));
	ImGui::SetNextWindowSize(ImVec2(nextWinSize.x, nextWinSize.y));

	ImGui::SetCursorPosX(15 + 60 * (itemNumber % maxNumberElements));
	ImGui::SetCursorPosY(iconSize.y + 120 + 72 * (itemNumber / maxNumberElements));


	ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 52);
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
	ImGuiContext* context = ImGui::GetCurrentContext();
	ImVec2 size = context->CurrentWindow->Size;

	int maxNumberElements = size.x / 60;
	if (maxNumberElements < 1) maxNumberElements = 1;

	ImGui::SetCursorPosX(15 + 60 * (itemNumber % maxNumberElements));
	ImGui::SetCursorPosY(180 + 72 * (itemNumber / maxNumberElements));

	// Select icon by extension
	std::string extension = App->fsystem->GetExtension(file);
	if		(extension == PNG)			{ ImGui::ImageButton(pngIcon != nullptr ? (ImTextureID)pngIcon->gpuID : 0, ImVec2(40, 40), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == TIF)			{ ImGui::ImageButton(tifIcon != nullptr ? (ImTextureID)tifIcon->gpuID : 0, ImVec2(40, 40), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == JPG)			{ ImGui::ImageButton(jpgIcon != nullptr ? (ImTextureID)jpgIcon->gpuID : 0, ImVec2(40, 40), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == TGA)			{ ImGui::ImageButton(tgaIcon != nullptr ? (ImTextureID)tgaIcon->gpuID : 0, ImVec2(iconSize.x, iconSize.y), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == TEXTUREEXT)	{ ImGui::ImageButton(ddsIcon != nullptr ? (ImTextureID)ddsIcon->gpuID : 0, ImVec2(iconSize.x, iconSize.y), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == FBXEXTENSION)	{ ImGui::ImageButton(fbxIcon != nullptr ? (ImTextureID)fbxIcon->gpuID : 0, ImVec2(iconSize.x, iconSize.y), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == MATERIALEXT)	{ ImGui::ImageButton(m4tIcon != nullptr ? (ImTextureID)m4tIcon->gpuID : 0, ImVec2(iconSize.x, iconSize.y), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else if (extension == JSONEXT)		{ ImGui::ImageButton(jsonIcon != nullptr ? (ImTextureID)jsonIcon->gpuID : 0, ImVec2(iconSize.x, iconSize.y), ImVec2(0, 1), ImVec2(1, 0), 1); }
	else								{ ImGui::ImageButton(fileIcon != nullptr ? (ImTextureID)fileIcon->gpuID : 0, ImVec2(iconSize.x, iconSize.y), ImVec2(0, 1), ImVec2(1, 0), 1); }

	if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		//fileSelected = path + file;
		fileSelected = App->resManager->GetWithoutLoad(App->fsystem->GetFilename(file).c_str());
		ImGui::OpenPopup("File Context Menu");
	}

	float2 nextWinSize = float2(350, 100);
	ImGui::SetNextWindowPos(ImVec2((App->window->width / 2) - nextWinSize.x / 2, (App->window->height / 2) - nextWinSize.y / 2));
	ImGui::SetNextWindowSize(ImVec2(nextWinSize.x, nextWinSize.y));

	ImGui::SetCursorPosX(15 + 60 * (itemNumber % maxNumberElements));
	ImGui::SetCursorPosY(iconSize.y + 180 + 72 * (itemNumber / maxNumberElements));

	ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 52);
	ImGui::Text(file);

	if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		//fileSelected = path + file;
		fileSelected = App->resManager->GetWithoutLoad(App->fsystem->GetFilename(file).c_str());
		ImGui::OpenPopup("File Context Menu");
	}
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
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			openImportConfigPopUp = false;
		}
		ImGui::EndPopup();
	}
}