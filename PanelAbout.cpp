#include "PanelAbout.h"
#include "SDL_version.h"
#include <windows.h>
#include "IL/il.h"
#include "assimp/version.h"
#include "GL/glew.h"
#include "Globals.h"

#define ENGINE_DESCRIPTION "Engine created during the Master of Videogames in UPC School."
#define AUTHOR "Albert Val Vila"
#define REPOSITORY "https://github.com/AlbertVVila/Engine"
#define LICENSE "https://github.com/AlbertVVila/Engine/blob/master/LICENSE"

PanelAbout::PanelAbout()
{
	enabled = false;
}


PanelAbout::~PanelAbout()
{
}

void PanelAbout::Draw()
{
	if (!ImGui::Begin("About", &enabled))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("Name:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), TITLE);
	ImGui::Separator();
	
	ImGui::Text("Description:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), ENGINE_DESCRIPTION);
	ImGui::Separator();
	
	if (ImGui::SmallButton("Github Repository")) {
		ShellExecuteA(NULL, "open", REPOSITORY, NULL, NULL, SW_SHOWNORMAL);
	};
	ImGui::Separator();
	
	ImGui::Text("Author:"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), AUTHOR);
	ImGui::Separator();
	
	SDL_version sdlVersion;
	SDL_GetVersion(&sdlVersion);

	if (ImGui::TreeNode("Libraries"))
	{
		ImGui::BulletText("SDL (version %d.%d.%d)", sdlVersion.major, sdlVersion.minor, sdlVersion.patch);
		ImGui::BulletText("Imgui (version %s)", ImGui::GetVersion());
		ImGui::BulletText("MathGeoLib (version 1.5)");
		ImGui::BulletText("glew (version %s)", glewGetString(GLEW_VERSION));
		ImGui::BulletText("Assimp (version %d.%d.%d)", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());
		ImGui::BulletText("DevIL (version %d)", IL_VERSION);

		ImGui::TreePop();
	}
	ImGui::Separator();
	if (ImGui::SmallButton("License")) {
		ShellExecuteA(NULL, "open", LICENSE, NULL, NULL, SW_SHOWNORMAL);
	};
	ImGui::Separator();
	ImGui::End();
}
