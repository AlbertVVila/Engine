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

#include "MathGeoLib\include\Math\float2.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "GUICreator.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleInput.h"
#include "GameObject.h"

#define FOLDER_ICON "FolderIcon"
#define FILE_ICON "FileIcon"
#define FBX_ICON "FBXFileIcon"

PanelBrowser::PanelBrowser() : path(ASSETS)
{
}

PanelBrowser::~PanelBrowser() 
{

}

bool PanelBrowser::Init()
{
	folderIcon = (ResourceTexture*)App->resManager->Get(FOLDER_ICON);
	fileIcon = (ResourceTexture*)App->resManager->Get(FILE_ICON);
	FBXIcon = (ResourceTexture*)App->resManager->Get(FBX_ICON);
	return true;
}

void PanelBrowser::Draw() 
{
	if (!ImGui::Begin("Assets", &enabled))
	{
		ImGui::End();
		return;
	}

	std::vector<std::string> files;
	std::vector<std::string> dirs;

	App->fsystem->ListFolderContent(path.c_str(), files, dirs);	
		
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(.0f, 0.5f));
	
	ImGui::Text(path.c_str());

	// If we are inside a folder show icon to go back
	if(path != ASSETS)
	{
		ImGui::ImageButton((ImTextureID)folderIcon->gpuID, ImVec2(40, 40), ImVec2(0, 1), ImVec2(1, 0), 1);
		if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			path = pathStack.top();
			pathStack.pop();
		}
		
		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 52);
		ImGui::Text("..");
		if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			path = pathStack.top();
			pathStack.pop();
		}
		ImGui::SetCursorPosX(15);
		ImGui::SetCursorPosY(10);
	}

	int i = 0;

	for each(std::string dir in dirs)
	{
		ImGuiContext* context = ImGui::GetCurrentContext();
		ImVec2 size = context->CurrentWindow->Size;
		int max_number_elements = size.x / 60;
		if (max_number_elements < 1) max_number_elements = 1;

		ImGui::SetCursorPosX(15 + 60 * (i%max_number_elements));
		ImGui::SetCursorPosY(120 + 72 * (i / max_number_elements));

		ImGui::ImageButton((ImTextureID)folderIcon->gpuID, ImVec2(40, 40), ImVec2(0, 1), ImVec2(1, 0), 1);
		
		if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			pathStack.push(path);
			path += dir + "/";
		}

		float2 next_win_size = float2(350, 100);
		ImGui::SetNextWindowPos(ImVec2((App->window->width / 2) - next_win_size.x / 2, (App->window->height / 2) - next_win_size.y / 2));
		ImGui::SetNextWindowSize(ImVec2(next_win_size.x, next_win_size.y));

		int image_height = 40;
		ImGui::SetCursorPosX(15 + 60*(i%max_number_elements));
		ImGui::SetCursorPosY(image_height + 120 + 72*(i / max_number_elements));

		
		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 52);
		ImGui::Text(dir.c_str());

		if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			pathStack.push(path);
			path += dir + "/";
		}
		
		i++;
	}
	
	int j = 0;
	for each (std::string file in files)
	{
		ImGuiContext* context = ImGui::GetCurrentContext();
		ImVec2 size = context->CurrentWindow->Size;
		int max_number_elements = size.x / 60;
		if (max_number_elements < 1) max_number_elements = 1;

		ImGui::SetCursorPosX(15 + 60 * (j%max_number_elements));
		ImGui::SetCursorPosY(180 + 72 * (j / max_number_elements));

		ImGui::ImageButton((ImTextureID)fileIcon->gpuID, ImVec2(40, 40), ImVec2(0, 1), ImVec2(1, 0), 1);

		if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
		{
			fileSelected = path + file;
			ImGui::OpenPopup("File Context Menu");
		}

		float2 next_win_size = float2(350, 100);
		ImGui::SetNextWindowPos(ImVec2((App->window->width / 2) - next_win_size.x / 2, (App->window->height / 2) - next_win_size.y / 2));
		ImGui::SetNextWindowSize(ImVec2(next_win_size.x, next_win_size.y));

		int image_height = 40;
		ImGui::SetCursorPosX(15 + 60 * (j%max_number_elements));
		ImGui::SetCursorPosY(image_height + 180 + 72 * (j / max_number_elements));

		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 52);
		ImGui::Text((file).c_str());

		if (ImGui::IsItemHovered() && App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
		{
			fileSelected = path + file;
			ImGui::OpenPopup("File Context Menu");
		}

		j++;
	}

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
			App->fsystem->Remove(fileSelected.c_str());
			//ImGui::EndPopup();
		}
		if (ImGui::Selectable("Import"), false)
		{
			//Code to import file
			//ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("Rename File"))
		{
			ImGui::Text("Write the new name for the file");
			char* newName = new char[30];
			memset(newName, 0, 30);
			size_t pos1 = 0;
			size_t pos2 = 0;
			std::vector<std::string> result;
			char separator = '/';
			while (pos2 != fileSelected.npos)
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
			}
			ImGui::EndPopup();
		}

		ImGui::EndPopup();
	}


	ImGui::PopStyleVar();
	ImGui::End();	
}