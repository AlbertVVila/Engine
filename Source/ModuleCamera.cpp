#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleTime.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleEditor.h"
#include "ModuleRender.h"

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
		InputMove();
		InputRotate();
		InputCenter();
		InputOrbit();
		InputZoom();
	}
	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	return editorcamera->CleanUp();
}

void ModuleCamera::InputMove() const
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		float3 movement = float3::zero;

		if (App->input->IsKeyPressed(SDL_SCANCODE_Q))
		{
			movement += float3::unitY;
		}
		if (App->input->IsKeyPressed(SDL_SCANCODE_E))
		{
			movement -= float3::unitY;
		}
		if (App->input->IsKeyPressed(SDL_SCANCODE_S))
		{
			movement -= editorcamera->frustum->front;
		}
		if (App->input->IsKeyPressed(SDL_SCANCODE_W))
		{
			movement += editorcamera->frustum->front;
		}
		if (App->input->IsKeyPressed(SDL_SCANCODE_A))
		{
			movement -= editorcamera->frustum->WorldRight();
		}
		if (App->input->IsKeyPressed(SDL_SCANCODE_D))
		{
			movement += editorcamera->frustum->WorldRight();
		}
		if (App->input->IsKeyPressed(SDL_SCANCODE_LSHIFT))
		{
			movement *= 2;
		}

		editorcamera->Move(movement*App->time->dt*App->renderer->current_scale);
	}
}

void ModuleCamera::InputRotate() const//TODO: Use deltatime
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		float dx = editorcamera->rotationSpeed * App->input->GetMouseMotion().x; //* App->time->dt;
		float dy = editorcamera->rotationSpeed * App->input->GetMouseMotion().y; //* App->time->dt;
		editorcamera->Rotate(dx,dy);
	}
}

void ModuleCamera::InputCenter() const
{
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		editorcamera->Center();
	}
}

void ModuleCamera::InputOrbit() const
{
	if (App->input->IsKeyPressed(SDL_SCANCODE_LALT) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		editorcamera->Orbit(editorcamera->rotationSpeed * App->input->GetMouseMotion().x,
			editorcamera->rotationSpeed * App->input->GetMouseMotion().y);
	}
}

void ModuleCamera::InputZoom() const
{
	editorcamera->Zoom(App->input->GetMouseWheel());
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
