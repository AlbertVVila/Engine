#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleTime.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleSceneLoader.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "GameObject.h"
#include "imgui.h"
#include "Math/MathFunc.h"
#include "Geometry/AABB.h"
#include "ComponentCamera.h"

ModuleCamera::ModuleCamera()
{
	editorcamera = new ComponentCamera();
}

ModuleCamera::~ModuleCamera()
{
}


update_status ModuleCamera::Update()
{
	if (App->editor->IsCameraFocused())
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			editorcamera->Move();
			editorcamera->Rotate();
		}
		if (App->input->IsKeyPressed(SDL_SCANCODE_F))
		{
			editorcamera->Center();
		}
		if (App->input->IsKeyPressed(SDL_SCANCODE_LALT) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
		{
			editorcamera->Orbit();
		}
		editorcamera->Zoom();
	}
	return UPDATE_CONTINUE;
}

void ModuleCamera::DrawGUI()
{
	ImGui::InputFloat3("Position", (float*)&editorcamera->cameraPos, 2, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat3("Forward ", (float*)&editorcamera->cameraFront, 2, ImGuiInputTextFlags_ReadOnly);

	ImGui::InputFloat("Movement Speed", &editorcamera->movementSpeed, 1.f, 5.f);
	ImGui::InputFloat("Rotation Speed", &editorcamera->rotationSpeed, 1.f, 5.f);
	ImGui::InputFloat("Zoom Speed", &editorcamera->zoomSpeed, 1.f, 5.f);
}
