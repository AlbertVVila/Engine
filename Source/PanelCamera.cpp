#include "Application.h"
#include "PanelCamera.h"

#include "ModuleTextures.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"

#include "ComponentCamera.h"
#include "imgui.h"

PanelCamera::PanelCamera()
{
}


PanelCamera::~PanelCamera()
{
}

void PanelCamera::Draw()
{
	ImGui::Begin("Game", &enabled, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	
	if (App->scene->maincamera != nullptr)
	{
		viewport.Draw(App->scene->maincamera);
	}
	else
	{
		ImVec2 size = ImGui::GetWindowSize();
		size.x = MAX(size.x, 400);
		size.y = MAX(size.y, 400);

		ImGui::Image((ImTextureID)App->scene->camera_notfound_texture->id,
			size, { 0,1 }, { 1,0 });
	}

	ImGui::End();
}
