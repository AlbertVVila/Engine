#include "ComponentCamera.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleTime.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"
#include "GameObject.h"
#include "Math/MathFunc.h"
#include "Geometry/AABB.h"
#include "GL/glew.h"

#define MAXFOV 120
#define MINFOV 40

ComponentCamera::ComponentCamera() : Component(nullptr, ComponentType::Camera)
{
	InitFrustum();
	CreateFrameBuffer();
}

ComponentCamera::ComponentCamera(GameObject * gameobject) : Component(gameobject, ComponentType::Camera)
{
	InitFrustum();
	CreateFrameBuffer();
}


ComponentCamera::~ComponentCamera()
{
	glDeleteFramebuffers(1, &FBO); //TODO: Remove cleanup on components to destructor
	glDeleteRenderbuffers(1, &RBO); //TODO: Destructors not called
	glDeleteTextures(1, &renderedTexture);
}

ComponentCamera * ComponentCamera::Clone()
{
	return new ComponentCamera(*this);
}

void ComponentCamera::InitFrustum()
{
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = float3::zero;
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 1000.0f;
	frustum.verticalFov = math::pi / 2.0f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * ((float)App->window->width / (float)App->window->height));

}

void ComponentCamera::Move()
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
		cameraPos -= cameraFront.Cross(float3::unitY).Normalized() * distance;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_D))
	{
		cameraPos += cameraFront.Cross(float3::unitY).Normalized() * distance;
	}
}
//TODO: Use mouse position + deltatime and not mouse motion
void ComponentCamera::Rotate()
{
	float deltaPitch = App->input->GetMouseMotion().y*rotationSpeed;
	pitch -= deltaPitch;
	if (pitch > 0)
	{
		pitch = MIN(89, pitch);
	}
	else
	{
		pitch = MAX(-89, pitch);
	}
	yaw += App->input->GetMouseMotion().x*rotationSpeed;
	ComputeEulerAngles();
}

void ComponentCamera::Zoom()
{
	float mouse_wheel = App->input->GetMouseWheel();
	if (mouse_wheel != 0)
	{
		frustum.verticalFov = mouse_wheel > 0 ?
			MAX(math::DegToRad(MINFOV), frustum.verticalFov - mouse_wheel * zoomSpeed) :
			MIN(math::DegToRad(MAXFOV), frustum.verticalFov - mouse_wheel * zoomSpeed);
		frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * ((float)App->window->width / (float)App->window->height));
	}
}

void ComponentCamera::Center()
{
	if (App->scene->selected == nullptr) return;

	AABB bbox = App->scene->selected->GetBoundingBox();
	float3 HalfSize = bbox.HalfSize();
	float distX = HalfSize.x / tanf(frustum.horizontalFov*0.5f);
	float distY = HalfSize.y / tanf(frustum.verticalFov*0.5f);
	float camDist = MAX(distX, distY) + HalfSize.z; //camera distance from model

	float3 center = bbox.FaceCenterPoint(5);
	cameraPos = center + float3(0, 0, camDist);

	cameraFront = float3(0, 0, -1);
	pitch = 0;
	yaw = -90;
}

void ComponentCamera::ComputeEulerAngles()
{// ¡Viva Euler, muerte al Quaternion!
	cameraFront.x = cos(math::DegToRad(yaw)) * cos(math::DegToRad(pitch));
	cameraFront.y = sin(math::DegToRad(pitch));
	cameraFront.z = sin(math::DegToRad(yaw)) *cos(math::DegToRad(pitch));
	cameraFront.Normalize();
}

void ComponentCamera::Orbit()
{
	if (App->scene->selected == nullptr) return;
	orbitX += App->input->GetMouseMotion().x*rotationSpeed;
	orbitY = MIN(89, orbitY + App->input->GetMouseMotion().y*rotationSpeed);

	AABB bbox = App->scene->selected->GetBoundingBox();
	radius = bbox.CenterPoint().Distance(cameraPos);

	cameraPos.x = cos(math::DegToRad(orbitX)) * cos(math::DegToRad(orbitY)) * radius;
	cameraPos.y = sin(math::DegToRad(orbitY)) * radius;;
	cameraPos.z = sin(math::DegToRad(orbitX)) *cos(math::DegToRad(orbitY)) * radius;
	cameraPos += bbox.CenterPoint();

	cameraFront = (bbox.CenterPoint() - cameraPos).Normalized();

	yaw = math::RadToDeg(atan2(cameraFront.z, cameraFront.x));
	pitch = math::RadToDeg(asin(cameraFront.y));
}

void ComponentCamera::Resize(float width, float height)
{
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * (width / height));
	CreateFrameBuffer(); //We recreate framebuffer with new window size
}

float4x4 ComponentCamera::GetViewMatrix()
{
	math::float4x4 view = LookAt(cameraPos, cameraPos + cameraFront, float3::unitY);
	return view.Transposed();
}

float4x4 ComponentCamera::GetProjectionMatrix()
{
	return frustum.ProjectionMatrix().Transposed();
}

float4x4 ComponentCamera::LookAt(math::float3 OBS, math::float3 VRP, math::float3 up) const
{
	math::float3 forward(VRP - OBS); forward.Normalize(); //deprecated with camerafront pos and up
	math::float3 side(forward.Cross(up)); side.Normalize();
	math::float3 u(side.Cross(forward));

	math::float4x4 matrix(math::float4x4::zero);
	matrix[0][0] = side.x; matrix[0][1] = side.y; matrix[0][2] = side.z;
	matrix[1][0] = u.x; matrix[1][1] = u.y; matrix[1][2] = u.z;
	matrix[2][0] = -forward.x; matrix[2][1] = -forward.y; matrix[2][2] = -forward.z;
	matrix[0][3] = -side.Dot(OBS); matrix[1][3] = -u.Dot(OBS); matrix[2][3] = forward.Dot(OBS);
	matrix[3][3] = 1;

	return matrix;
}

void ComponentCamera::CreateFrameBuffer()
{
	glDeleteFramebuffers(1, &FBO);
	glDeleteRenderbuffers(1, &RBO);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &renderedTexture);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGB, App->window->width, App->window->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0
	);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO); //TODO: Render reduced window not all window
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->window->width, App->window->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("Framebuffer ERROR");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

