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
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
	{
		cameraPos.y += cameraSpeed * App->time->deltaTime;
	}
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
	{
		cameraPos.y -= cameraSpeed * App->time->deltaTime;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		cameraPos -= cameraFront * cameraSpeed * App->time->deltaTime;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		cameraPos += cameraFront * cameraSpeed * App->time->deltaTime;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		cameraPos -= cameraFront.Cross(cameraUp).Normalized() * cameraSpeed * App->time->deltaTime;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		cameraPos += cameraFront.Cross(cameraUp).Normalized() * cameraSpeed * App->time->deltaTime;
	}
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		pitch = MIN(89, pitch + cameraSpeed * App->time->deltaTime * 20);
		ComputeEulerAngles();
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		pitch = MAX(-89, pitch - cameraSpeed * App->time->deltaTime * 20);
		ComputeEulerAngles();
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		yaw -= cameraSpeed * App->time->deltaTime * 20;
		ComputeEulerAngles();
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		yaw += cameraSpeed * App->time->deltaTime * 20;
		ComputeEulerAngles();
	}
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
