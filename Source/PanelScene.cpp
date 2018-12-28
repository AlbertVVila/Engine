#include "Application.h"

#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentCamera.h"

#include "PanelScene.h"

#include "imgui.h"
#include "ImGuizmo.h"
#include "Math/float4x4.h"
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
	ImGui::Image((ImTextureID)App->camera->editorcamera->camTexture, 
		{ (float)App->window->width, (float)App->window->height }, { 0,1 }, { 1,0 });

	focus = ImGui::IsWindowFocused();

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetDrawlist();

	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

	ImGui::SetCursorPos(ImGui::GetWindowContentRegionMin());

	if (ImGui::Button("Translate"))
	{
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::Button("Rotate"))
	{
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	}
	ImGui::SameLine();
	if (ImGui::Button("Scale"))
	{
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	}

	if (App->scene->selected != nullptr) //TODO: WORLD translation + rotation, scale? -> LOCAL do it with local !watch rotation
	{

		float4x4 model = App->scene->selected->GetGlobalTransform();
		float4x4 view = App->camera->editorcamera->GetViewMatrix();
		float4x4 proj = App->camera->editorcamera->GetProjectionMatrix();
		ImGuizmo::SetOrthographic(false);
		model.Transpose();
		//ImGuizmo::DrawCube((float*)&view, (float*)&proj, (float*)&model);
		ImGuizmo::Manipulate((float*)&view, (float*)&proj, mCurrentGizmoOperation, ImGuizmo::LOCAL, (float*)&model, NULL, NULL, NULL, NULL);
		model.Transpose();
			
		App->scene->selected->SetGlobalTransform(model);
	}
	ImGui::End();
}
