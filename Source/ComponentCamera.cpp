#include "ComponentCamera.h"
#include "ComponentTransform.h"

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
	frustum.pos =gameobject->GetBoundingBox().FaceCenterPoint(4);
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

void ComponentCamera::Move() //TODO Move with float3 translation
{
	float distance = movementSpeed * App->time->deltaTime;
	float3 movement = float3::zero;
	if (App->input->IsKeyPressed(SDL_SCANCODE_LSHIFT))
	{
		distance *= 2;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_Q))
	{
		movement+=float3::unitY*distance;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_E))
	{
		movement -= float3::unitY*distance;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_S))
	{
		movement -= frustum.front*distance;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_W))
	{
		movement += frustum.front*distance;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_A))
	{
		movement -= frustum.WorldRight()*distance;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_D))
	{
		movement += frustum.WorldRight()*distance;
	}
	frustum.Translate(movement);
}
//TODO: Use mouse position + deltatime and not mouse motion
void ComponentCamera::Rotate(float dx, float dy)
{
	if (dx != 0)
	{
		Quat rotation = Quat::RotateY(math::DegToRad(-dx)).Normalized();
		frustum.front = rotation.Mul(frustum.front).Normalized();
		frustum.up = rotation.Mul(frustum.up).Normalized();
	}
	if (dy != 0)
	{
		Quat rotation = Quat::RotateAxisAngle(frustum.WorldRight(), math::DegToRad(-dy)).Normalized();
		float3 valid_up = rotation.Mul(frustum.up).Normalized();
		if (valid_up.y > 0.0f)
		{
			frustum.up = valid_up;
			frustum.front = rotation.Mul(frustum.front).Normalized();
		}
	}
}


void ComponentCamera::Zoom(float mouseWheel)
{
	if (mouseWheel != 0)
	{
		frustum.verticalFov = mouseWheel > 0 ?
			MAX(math::DegToRad(MINFOV), frustum.verticalFov - mouseWheel * zoomSpeed) :
			MIN(math::DegToRad(MAXFOV), frustum.verticalFov - mouseWheel * zoomSpeed);
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
	frustum.pos = center + float3(0, 0, camDist);

	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
}


void ComponentCamera::Orbit(float dx, float dy)
{
	if (App->scene->selected == nullptr) return;

	AABB bbox = App->scene->selected->GetBoundingBox();
	float3 center = float3::zero;//bbox.CenterPoint();

	if (dx != 0)
	{
		Quat rotation = Quat::RotateY(math::DegToRad(-dx)).Normalized();
		frustum.pos = rotation.Mul(frustum.pos);
	}
	if (dy != 0)
	{
		Quat rotation = Quat::RotateAxisAngle(frustum.WorldRight(), math::DegToRad(-dy)).Normalized();
		float3 new_pos = rotation.Mul(frustum.pos);
		if (!(abs(new_pos.x-center.x) < 0.5f && abs(new_pos.z - center.z) < 0.5f))
		{
			frustum.pos = new_pos;
		}
	}
	LookAt(center);
}

void ComponentCamera::LookAt(float3 target)
{
	float3 dir = (target - frustum.pos).Normalized();
	float3x3 look = float3x3::LookAt(frustum.front, dir, frustum.up, float3::unitY);
	frustum.front = look.Mul(frustum.front).Normalized();
	frustum.up = look.Mul(frustum.up).Normalized();
}

void ComponentCamera::Resize(float width, float height)
{
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * (width / height));
	CreateFrameBuffer(); //We recreate framebuffer with new window size
}

void ComponentCamera::Update() 
{
	if (gameobject == nullptr) return;
	if (gameobject->transform == nullptr) return;

	frustum.pos = gameobject->GetBoundingBox().FaceCenterPoint(4);
} 

float4x4 ComponentCamera::GetViewMatrix()
{
	float4x4 view = frustum.ViewMatrix();
	return view.Transposed();
}

float4x4 ComponentCamera::GetProjectionMatrix()
{
	return frustum.ProjectionMatrix().Transposed();
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
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->window->width, App->window->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("Framebuffer ERROR");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



