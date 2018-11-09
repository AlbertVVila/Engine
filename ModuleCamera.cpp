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
	CameraInput();
	return UPDATE_CONTINUE;
}


bool ModuleCamera::CleanUp()
{
	return true;
}

void ModuleCamera::CameraInput()
{
	if (App->editor->IsCameraFocused())
	{
		Move();
		Rotate();
		Center();
		Zoom();
		Orbit();
	}
}



void ModuleCamera::Move()
{
	float distance = movementSpeed * App->time->deltaTime;
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
	float rotation = rotationSpeed * App->time->deltaTime;
	if (App->input->IsKeyPressed(SDL_SCANCODE_UP))
	{
		pitch = MIN(89, pitch + rotation);
		ComputeEulerAngles();
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_DOWN))
	{
		pitch = MAX(-89, pitch - rotation);
		ComputeEulerAngles();
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_LEFT))
	{
		yaw -= rotation;
		ComputeEulerAngles();
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_RIGHT))
	{
		yaw += rotation;
		ComputeEulerAngles();
	}
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
	if (App->input->IsKeyPressed(SDL_SCANCODE_F) && App->model->models.size() > 0)
	{
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
	if (App->input->GetMousePosition().x != 0 && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
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
}
