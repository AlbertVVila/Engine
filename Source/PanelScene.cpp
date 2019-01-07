#include "Application.h"

#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentCamera.h"

#include "PanelScene.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"
#include "Geometry/LineSegment.h"
PanelScene::PanelScene()
{
}


PanelScene::~PanelScene()
{
}

void PanelScene::Draw()
{
	ImGui::Begin("Scene", &enabled, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	viewport.Draw(App->camera->editorcamera, true);
	focus = ImGui::IsWindowFocused();

	ImGui::End();
}
