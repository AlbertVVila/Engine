#include "FileExplorer.h"

#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"

#include "imgui/imgui.h"

FileExplorer::FileExplorer()
{
	extensionToFilter = FILETYPE::NONE;
}

FileExplorer::~FileExplorer()
{
}

void FileExplorer::Reset()
{
	path = "";
	extensionToFilter = FILETYPE::NONE;
	while (!pathStack.empty())
		pathStack.pop();
}

bool FileExplorer::Open()
{
	if (ImGui::BeginPopupModal(title, &openFileExplorer))
	{
		DrawPath();
		// List of files and directories box: [..] [Directories] [Files]
		std::vector<std::string> files;
		std::vector<std::string> dirs;
		ImGui::BeginChild("Files", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowContentRegionMax().y - 120), true, ImGuiWindowFlags_HorizontalScrollbar);
		App->fsystem->ListFolderContent(path.c_str(), files, dirs);

		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(.0f, 0.5f));
		// [..] (Goes back one dir)
		if (pathStack.size() > 0)
		{
			if (ImGui::Selectable("..", false))
			{
				path = pathStack.top();
				pathStack.pop();
			}
		}
		// [Directories]
		for each (std::string s in dirs)
		{
			std::string slash_dir = s + "/";
			if (ImGui::Selectable(slash_dir.c_str(), false))
			{
				pathStack.push(path);
				path = path + "/" + s;
			}
		}
		// [Files]
		for each (std::string s in files)
		{
			std::string extension = "";
			switch (extensionToFilter)
			{
			case FILETYPE::TEXTURE:		
				extension = App->fsystem->GetExtension(s);
				if (extension == TEXTUREEXT || extension == PNG || extension == TIF || extension == JPG)
				{
					if (ImGui::Selectable(s.c_str(), false))
						sprintf_s(file, App->fsystem->RemoveExtension(s).c_str());
				}
				break;
			case FILETYPE::MODEL:
				extension = App->fsystem->GetExtension(s);
				if (extension == FBXEXTENSION || extension == FBXCAPITAL)
				{
					if (ImGui::Selectable(s.c_str(), false))
						sprintf_s(file, App->fsystem->RemoveExtension(s).c_str());
				}
				break;
			case FILETYPE::MESH:
				extension = App->fsystem->GetExtension(s);
				if (extension == MESHEXTENSION)
				{
					if (ImGui::Selectable(s.c_str(), false))
						sprintf_s(file, App->fsystem->RemoveExtension(s).c_str());
				}
				break;
			case FILETYPE::SCENE:
				extension = App->fsystem->GetExtension(s);
				if (extension == JSONEXT)
				{
					if (ImGui::Selectable(s.c_str(), false))
						sprintf_s(file, App->fsystem->RemoveExtension(s).c_str());
				}
				break;
			default:
			case FILETYPE::NONE:
				if (ImGui::Selectable(s.c_str(), false))
					sprintf_s(file, App->fsystem->RemoveExtension(s).c_str());
				break;
			}
		}
		ImGui::PopStyleVar();
		ImGui::EndChild();

		// File bar: [Text (File name:)] [Text input for filename] [Combo selector to filter extensions] 
		ImGui::Text("File Name:");
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() * 0.55f);
		ImGui::InputText("", &file[0], sizeof(file));
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() * 0.3f);
		char* program_names[(int)FILETYPE::NONE+1] = { "Texture Files (*.png, *.tif, *.jpg, *.dds)", "Model Files (*.fbx, *.FBX)", "Imported Mesh Files (*.m3sh)", "Scene Files (*.json)", "All files" };
		ImGui::Combo(" ", (int*)&extensionToFilter, program_names, (int)FILETYPE::NONE + 1);
		ImGui::PopItemWidth();

		// Accept and Cancel buttons
		if (ImGui::Button("Accept", ImVec2(100, 20)))
		{
			openFileExplorer = false;
			ImGui::EndPopup();
			return true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(100, 20)))
		{
			openFileExplorer = false;
			ImGui::EndPopup();
			return false;
		}
		ImGui::EndPopup();
	}
	return false;
}

void FileExplorer::Draw()
{
	if (openFileExplorer)
	{
		ImGui::SetNextWindowSizeConstraints(ImVec2(600, 300), ImVec2(1200, 800));
		ImGui::OpenPopup(title);

		if (Open())
		{
			switch (currentOperation)
			{
			case MenuOperations::SAVE:
				App->scene->SaveScene(*App->scene->root, file);
				currentOperation = MenuOperations::NONE;
				break;
			case MenuOperations::LOAD:
				App->scene->LoadScene(file);
				currentOperation = MenuOperations::NONE;
				break;
			}
		}
	}
}

void FileExplorer::DrawPath()
{
	std::vector<int> separatorLocations;
	std::vector<std::string> folders;
	for (int i = 0; i < path.size(); i++)
		if (path[i] == '/')
			separatorLocations.push_back(i);
	for (std::vector<int>::iterator it = separatorLocations.begin(); it != separatorLocations.end(); ++it)
	{
		if (it != separatorLocations.end() - 1)
		{
			folders.push_back(path.substr((*it), (*(it + 1)) - (*it)));
		}
		else {
			folders.push_back(path.substr((*it), (path.size() - (*it))));
		}
	}
	if (ImGui::Button("."))
	{
		Reset();
	}
	for each (std::string folder in folders)
	{
		ImGui::SameLine();
		if (ImGui::Button(folder.c_str()))
		{
			if (folder != folders[folders.size() - 1])
			{
				while (pathStack.top().substr(pathStack.top().find_last_of("/")) != folder)
				{
					pathStack.pop();
				}
				if (pathStack.top().substr(pathStack.top().find_last_of("/")) == folder)
				{
					path = pathStack.top();
					pathStack.pop();
				}
			}
		}
	}
}