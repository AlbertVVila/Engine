#include "FileExplorer.h"

#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"

#include "imgui/imgui.h"

FileExplorer::FileExplorer()
{
}

FileExplorer::~FileExplorer()
{
}

void FileExplorer::Reset()
{
	path = App->fsystem->baseDir;
	while (!pathStack.empty())
		pathStack.pop();
}

bool FileExplorer::Open()
{
	if (ImGui::BeginPopupModal(title, &openFileExplorer))
	{
		ImGui::Text((path + "/").c_str());

		std::vector<std::string> files;
		std::vector<std::string> dirs;
		ImGui::BeginChild("Files", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowContentRegionMax().y - 120), true, ImGuiWindowFlags_HorizontalScrollbar);
		App->fsystem->ListFolderContent(path.c_str(), files, dirs);

		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(.0f, 0.5f));
		if (pathStack.size() > 0)
		{
			if (ImGui::Selectable("..", false))
			{
				path = pathStack.top();
				pathStack.pop();
			}
		}
		for each (std::string s in dirs)
		{
			std::string slash_dir = s + "/";
			if (ImGui::Selectable(slash_dir.c_str(), false))
			{
				pathStack.push(path);
				path = path + "/" + s;
			}
		}

		for each (std::string s in files)
		{
			if (ImGui::Selectable(s.c_str(), false))
				sprintf_s(file, s.c_str());
		}
		ImGui::PopStyleVar();
		ImGui::EndChild();

		ImGui::InputText("File Name", &file[0], sizeof(file));

		// Accept and Cancel buttons
		if (ImGui::Button("Accept", ImVec2(100, 20)))
		{
			//ImGui::CloseCurrentPopup();
			openFileExplorer = false;
			ImGui::EndPopup();
			return true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(100, 20)))
		{
			//ImGui::CloseCurrentPopup();
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
		ImGui::SetNextWindowSizeConstraints(ImVec2(500, 300), ImVec2(800, 800));
		ImGui::OpenPopup(title);

		if (Open())
		{
			std::string filename;
			switch (currentOperation)
			{
			case MenuOperations::SAVE:
				filename = App->fsystem->GetFilename(file);
				App->scene->SaveScene(*App->scene->root, filename.c_str());
				currentOperation = MenuOperations::NONE;
				break;
			case MenuOperations::LOAD:
				filename = App->fsystem->GetFilename(file);
				App->scene->LoadScene(filename.c_str());
				currentOperation = MenuOperations::NONE;
				break;
			}
		}
	}

	
}