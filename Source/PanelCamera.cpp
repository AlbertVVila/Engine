#include "PanelCamera.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"
#include "ComponentCamera.h"

PanelCamera::PanelCamera()
{
}


PanelCamera::~PanelCamera()
{
}

void PanelCamera::Draw()
{
	ImGui::Begin("Game", &enabled, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	ImVec2 size = ImGui::GetWindowSize();
	ImGui::SetCursorPos({ -(App->window->width - size.x) / 2,-(App->window->height - size.y) / 2 });
	if (App->scene->maincamera != nullptr)
	{
		ImGui::Image((ImTextureID)App->scene->maincamera->camTexture,
			{ (float)App->window->width, (float)App->window->height }, { 0,1 }, { 1,0 });
	}
	else
	{
		ImGui::Image((ImTextureID)App->scene->camera_notfound_texture->id,
			{ (float)App->window->width, (float)App->window->height }, { 0,1 }, { 1,0 });
	}
	
	ImGui::End();
}
