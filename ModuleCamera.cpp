#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleTime.h"
#include "ModuleRender.h"

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
	ProcessInput();
	return UPDATE_CONTINUE;
}


bool ModuleCamera::CleanUp()
{
	return true;
}

void ModuleCamera::ProcessInput()
{
	Move();
	Rotate();
	Zoom();
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

void ModuleCamera::ComputeEulerAngles()
{
	cameraFront.x = cos(math::DegToRad(yaw)) * cos(math::DegToRad(pitch));
	cameraFront.y = sin(math::DegToRad(pitch));
	cameraFront.z = sin(math::DegToRad(yaw)) *cos(math::DegToRad(pitch));
	cameraFront.Normalize();

}
