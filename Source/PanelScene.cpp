#include "PanelScene.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"

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
	ImGui::Image((ImTextureID)App->renderer->renderTexture, 
		{ (float)App->window->width, (float)App->window->height }, { 0,1 }, { 1,0 });

	focus = ImGui::IsWindowFocused();
	ImGui::End();
}

bool PanelScene::IsFocused() const
{
	return focus;
}
