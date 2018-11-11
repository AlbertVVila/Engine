#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleTime.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleModel.h"
#include "ModuleEditor.h"
#include "imgui.h"

#define MAXFOV 120
#define MINFOV 40

ModuleCamera::ModuleCamera()
{
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
			Move();
			Rotate();
		}
		if (App->input->IsKeyPressed(SDL_SCANCODE_F))
		{
			Center();
		}
		if (App->input->IsKeyPressed(SDL_SCANCODE_LALT) && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
		{
			Orbit();
		}
		Zoom();
	}
	return UPDATE_CONTINUE;
}

void ModuleCamera::Move()
{
	float distance = movementSpeed * App->time->deltaTime;
	if (App->input->IsKeyPressed(SDL_SCANCODE_LSHIFT))
	{
		distance *= 2;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_Q))
	{
		cameraPos.y += distance;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_E))
	{
		cameraPos.y -= distance;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_S))
	{
		cameraPos -= cameraFront * distance;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_W))
	{
		cameraPos += cameraFront * distance;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_A))
	{
		cameraPos -= cameraFront.Cross(cameraUp).Normalized() * distance;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_D))
	{
		cameraPos += cameraFront.Cross(cameraUp).Normalized() * distance;
	}
}
//TODO: Use mouse position + deltatime and not mouse motion
void ModuleCamera::Rotate()
{
	float deltaPitch = App->input->GetMouseMotion().y*rotationSpeed; 
	pitch = deltaPitch <0? MAX(-89, pitch - deltaPitch) : MIN(89, pitch - deltaPitch);

	yaw += App->input->GetMouseMotion().x*rotationSpeed;
	ComputeEulerAngles();
}

void ModuleCamera::Zoom()
{
	if (App->input->mouse_wheel != 0)
	{
		App->renderer->frustum.verticalFov = App->input->mouse_wheel > 0 ?
			MAX(math::DegToRad(MINFOV), App->renderer->frustum.verticalFov - App->input->mouse_wheel*zoomSpeed) :
			MIN(math::DegToRad(MAXFOV), App->renderer->frustum.verticalFov - App->input->mouse_wheel*zoomSpeed);
		App->renderer->frustum.horizontalFov = 2.f * atanf(tanf(App->renderer->frustum.verticalFov * 0.5f) * ((float)App->window->width / (float)App->window->height));
	}
}

void ModuleCamera::Center()
{
	if (App->model->models.size() == 0) return;

	float3 HalfSize = App->model->models.front().BoundingBox.HalfSize();
	float distX = HalfSize.x / tanf(App->renderer->frustum.horizontalFov*0.5f);
	float distY = HalfSize.y / tanf(App->renderer->frustum.verticalFov*0.5f); 
	float camDist = MAX(distX,distY) + HalfSize.z;

	float3 center = App->model->models.front().BoundingBox.FaceCenterPoint(5);
	cameraPos = center + float3(0,0, camDist);

	cameraFront = float3(0, 0, -1);
	pitch = 0;
	yaw = -90;
}

void ModuleCamera::ComputeEulerAngles()
{// ¡Viva Euler, muerte al Quaternion!
	cameraFront.x = cos(math::DegToRad(yaw)) * cos(math::DegToRad(pitch));
	cameraFront.y = sin(math::DegToRad(pitch));
	cameraFront.z = sin(math::DegToRad(yaw)) *cos(math::DegToRad(pitch));
	cameraFront.Normalize();
}

void ModuleCamera::Orbit()
{
	if (App->model->models.size() == 0) return;
	orbitX += (App->input->GetMouseMotion().x*rotationSpeed);
	orbitY += (App->input->GetMouseMotion().y*rotationSpeed);

	radius = App->model->models.front().BoundingBox.CenterPoint().Distance(cameraPos);

	cameraPos.x = cos(math::DegToRad(orbitX)) * cos(math::DegToRad(orbitY)) * radius;
	cameraPos.y = sin(math::DegToRad(orbitY)) * radius;;
	cameraPos.z = sin(math::DegToRad(orbitX)) *cos(math::DegToRad(orbitY)) * radius;
	cameraPos += App->model->models.front().BoundingBox.CenterPoint();

	cameraFront = (App->model->models.front().BoundingBox.CenterPoint() - cameraPos).Normalized();

	yaw = math::RadToDeg(atan2(cameraFront.z, cameraFront.x));
	pitch = math::RadToDeg(asin(cameraFront.y));
}

void ModuleCamera::DrawGUI()
{
	ImGui::Text("Position "); ImGui::SameLine();
	ImGui::Text("X: %.2f", cameraPos.x, ImGuiInputTextFlags_ReadOnly); ImGui::SameLine();
	ImGui::Text("Y: %.2f", cameraPos.y, ImGuiInputTextFlags_ReadOnly); ImGui::SameLine();
	ImGui::Text("Z: %.2f", cameraPos.z, ImGuiInputTextFlags_ReadOnly); 

	ImGui::Text("Forward "); ImGui::SameLine();
	ImGui::Text("X: %.2f", cameraFront.x, ImGuiInputTextFlags_ReadOnly); ImGui::SameLine();
	ImGui::Text("Y: %.2f", cameraFront.y, ImGuiInputTextFlags_ReadOnly); ImGui::SameLine();
	ImGui::Text("Z: %.2f", cameraFront.z, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat("Movement Speed", &movementSpeed, 1.f, 5.f);
	ImGui::InputFloat("Rotation Speed", &rotationSpeed, 1.f, 5.f);
	ImGui::InputFloat("Zoom Speed", &zoomSpeed, 1.f, 5.f);
}
