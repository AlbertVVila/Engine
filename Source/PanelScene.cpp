#include "PanelScene.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ComponentCamera.h"
PanelScene::PanelScene()
{
}


PanelScene::~PanelScene()
{
}

void PanelScene::Draw()
{
	ImGui::Begin("Scene", &enabled, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	ImVec2 size = ImGui::GetWindowSize();
	ImGui::SetCursorPos({ -(App->window->width - size.x) / 2,-(App->window->height - size.y) / 2 });
	ImGui::Image((ImTextureID)App->camera->editorcamera->renderedTexture, 
		{ (float)App->window->width, (float)App->window->height }, { 0,1 }, { 1,0 });

	focus = ImGui::IsWindowFocused();
	ImGui::End(); //TODO: Remove or move to gitignore .exe file
}
