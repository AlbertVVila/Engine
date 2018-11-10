#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleTime.h"
#include "ModuleRender.h"
#include "ModuleModel.h"
#include "ModuleEditor.h"
ModuleCamera::ModuleCamera()
{
}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
    return true;
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


bool ModuleCamera::CleanUp()
{
	return true;
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

void ModuleCamera::Rotate()
{
	float rotation = rotationSpeed * App->time->deltaTime*4000;
	
	float deltaPitch = App->input->GetMouseMotion().y*rotation;
	pitch = deltaPitch <0? MAX(-89, pitch - deltaPitch) : MIN(89, pitch - deltaPitch);

	yaw += App->input->GetMouseMotion().x*rotation;
	ComputeEulerAngles();
}

void ModuleCamera::Zoom()
{
	if (App->input->mouse_wheel != 0)
	{
		App->renderer->frustum.verticalFov -= App->input->mouse_wheel *100.f*App->time->deltaTime;
		App->renderer->frustum.horizontalFov = 2.f * atanf(tanf(App->renderer->frustum.verticalFov * 0.5f) * ((float)App->renderer->width / (float)App->renderer->height));
	}
}

void ModuleCamera::Center()
{
	if (App->model->models.size() == 0) return;

	float3 HalfSize = App->model->models.front().BoundingBox.HalfSize();
	float distX = HalfSize.x / tanf(App->renderer->frustum.horizontalFov*0.5f);
	float distY = HalfSize.y / tanf(App->renderer->frustum.verticalFov*0.5f); //use x*x+y*y+z*z as dist
	float camDist = MAX(distX,distY);

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
	startAngleX += (App->input->GetMouseMotion().x*200);
	startAngleY += (App->input->GetMouseMotion().y*200);

	radius = App->model->models.front().BoundingBox.CenterPoint().Distance(cameraPos);

	cameraPos.x = cos(math::DegToRad(startAngleX)) * cos(math::DegToRad(startAngleY)) * radius;
	cameraPos.y = sin(math::DegToRad(startAngleY)) * radius;;
	cameraPos.z = sin(math::DegToRad(startAngleX)) *cos(math::DegToRad(startAngleY)) * radius;
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
	//TODO: move fov and znearfar here
}
