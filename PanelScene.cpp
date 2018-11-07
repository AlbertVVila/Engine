#include "PanelScene.h"
#include "Application.h"
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

	ImGui::Image((ImTextureID)App->renderer->renderTexture, 
		{ (float)App->renderer->width, (float)App->renderer->height }, { 0,1 }, { 1,0 });
	ImGui::End();
}
