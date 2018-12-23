#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleEditor.h"

#include "ComponentCamera.h"

#include "imgui.h"
#include "Math/MathFunc.h"
#include "Geometry/Frustum.h"

ModuleCamera::ModuleCamera()
{
}

ModuleCamera::~ModuleCamera()
{
	RELEASE(editorcamera);
}

bool ModuleCamera::Init()
{
	editorcamera = new ComponentCamera();
	return true;
}

update_status ModuleCamera::Update() //TODO: vsync bug rotation smooth
{
	if (App->editor->IsCameraFocused())
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			editorcamera->Move();
			editorcamera->Rotate(editorcamera->rotationSpeed * App->input->GetMouseMotion().x,
				editorcamera->rotationSpeed * App->input->GetMouseMotion().y);
		}
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		{
			editorcamera->Center();
		}
		if (App->input->IsKeyPressed(SDL_SCANCODE_LALT) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
		{
			editorcamera->Orbit(editorcamera->rotationSpeed * App->input->GetMouseMotion().x,
				editorcamera->rotationSpeed * App->input->GetMouseMotion().y);
		}
		editorcamera->Zoom(App->input->GetMouseWheel());
	}
	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	return editorcamera->CleanUp();
}

void ModuleCamera::DrawGUI()
{
	math::Frustum *frustum = editorcamera->frustum;
	ImGui::InputFloat3("Position", (float*)&frustum->pos, 2, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat3("Forward ", (float*)&frustum->front, 2, ImGuiInputTextFlags_ReadOnly);

	ImGui::InputFloat("Movement Speed", &editorcamera->movementSpeed, 1.f, 5.f);
	ImGui::InputFloat("Rotation Speed", &editorcamera->rotationSpeed, 1.f, 5.f);
	ImGui::InputFloat("Zoom Speed", &editorcamera->zoomSpeed, 1.f, 5.f);

	float degFov = math::RadToDeg(frustum->verticalFov);
	if (ImGui::SliderFloat("FOV", &degFov, 40, 120))
	{
		frustum->verticalFov = math::DegToRad(degFov);
		frustum->horizontalFov = 2.f * atanf(tanf(frustum->verticalFov*0.5f)*App->window->width / App->window->height);
	}
	ImGui::InputFloat("Znear", &frustum->nearPlaneDistance, 1, 10);
	ImGui::InputFloat("Zfar", &frustum->farPlaneDistance, 1, 10);
}
