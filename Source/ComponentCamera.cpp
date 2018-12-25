#include "Application.h"

#include "ModuleScene.h"
#include "ModuleWindow.h"
#include "ModuleProgram.h"

#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"

#include "JSON.h"
#include "Math/MathFunc.h"
#include "Geometry/AABB.h"
#include "GL/glew.h"
#include "Imgui/imgui.h"

#define MAXFOV 120
#define MINFOV 40
#define ZNEARDIST .1f
#define ZFARDIST 1000.f

ComponentCamera::ComponentCamera() : Component(nullptr, ComponentType::Camera)
{
	InitFrustum();
	CreateFrameBuffer();
}

ComponentCamera::ComponentCamera(GameObject * gameobject) : Component(gameobject, ComponentType::Camera)
{
	InitFrustum();
	CreateFrameBuffer();
	frustum->pos = gameobject->GetBoundingBox().CenterPoint();
}

ComponentCamera::ComponentCamera(const ComponentCamera & component) : Component(component)
{
	frustum = &math::Frustum(*component.frustum);
	movementSpeed = component.movementSpeed;
	rotationSpeed = component.rotationSpeed;
	zoomSpeed = component.zoomSpeed;

	camTexture = component.camTexture;
	FBO = component.FBO;
	RBO = component.RBO;
}


ComponentCamera::~ComponentCamera()
{
	if (frustum != nullptr)
	{
		RELEASE(frustum);
	}
}

ComponentCamera * ComponentCamera::Clone() const
{
	return new ComponentCamera(*this);
}

void ComponentCamera::InitFrustum()
{
	frustum = new math::Frustum();
	frustum->type = FrustumType::PerspectiveFrustum;
	frustum->pos = float3::zero;
	frustum->front = -float3::unitZ;
	frustum->up = float3::unitY;
	frustum->nearPlaneDistance = ZNEARDIST;
	frustum->farPlaneDistance = ZFARDIST;
	frustum->verticalFov = math::pi / 2.0f;
	frustum->horizontalFov = 2.f * atanf(tanf(frustum->verticalFov * 0.5f) * ((float)App->window->width / (float)App->window->height));

}

void ComponentCamera::Move(float3 dir)
{
	frustum->Translate(dir*movementSpeed);
}

void ComponentCamera::Rotate(float dx, float dy)
{
	if (dx != 0)
	{
		Quat rotation = Quat::RotateY(math::DegToRad(-dx)).Normalized();
		frustum->front = rotation.Mul(frustum->front).Normalized();
		frustum->up = rotation.Mul(frustum->up).Normalized();
	}
	if (dy != 0)
	{
		Quat rotation = Quat::RotateAxisAngle(frustum->WorldRight(), math::DegToRad(-dy)).Normalized();
		float3 valid_up = rotation.Mul(frustum->up).Normalized();
		if (valid_up.y > 0.0f)
		{
			frustum->up = valid_up;
			frustum->front = rotation.Mul(frustum->front).Normalized();
		}
	}
}


void ComponentCamera::Zoom(float mouseWheel)
{
	if (mouseWheel != 0)
	{
		frustum->verticalFov = mouseWheel > 0 ?
			MAX(math::DegToRad(MINFOV), frustum->verticalFov - mouseWheel * zoomSpeed) :
			MIN(math::DegToRad(MAXFOV), frustum->verticalFov - mouseWheel * zoomSpeed);
		frustum->horizontalFov = 2.f * atanf(tanf(frustum->verticalFov * 0.5f) * ((float)App->window->width / (float)App->window->height));
	}
}

void ComponentCamera::Center()
{
	if (App->scene->selected == nullptr) return;

	AABB bbox = App->scene->selected->GetBoundingBox();
	float3 HalfSize = bbox.HalfSize();
	float distX = HalfSize.x / tanf(frustum->horizontalFov*0.5f);
	float distY = HalfSize.y / tanf(frustum->verticalFov*0.5f);
	float camDist = MAX(distX, distY) + HalfSize.z; //camera distance from model

	float3 center = bbox.FaceCenterPoint(5);
	frustum->pos = center + float3(0, 0, camDist);

	frustum->front = -float3::unitZ;
	frustum->up = float3::unitY;
}


void ComponentCamera::Orbit(float dx, float dy)
{
	if (App->scene->selected == nullptr) return;

	AABB bbox = App->scene->selected->GetBoundingBox();
	float3 center = bbox.CenterPoint();

	if (dx != 0)
	{
		Quat rotation = Quat::RotateY(math::DegToRad(-dx)).Normalized();
		frustum->pos = rotation.Mul(frustum->pos - center) + center;
	}
	if (dy != 0)
	{
		Quat rotation = Quat::RotateAxisAngle(frustum->WorldRight(), math::DegToRad(-dy)).Normalized();
		float3 new_pos = rotation.Mul(frustum->pos - center) + center;
		if (!(abs(new_pos.x-center.x) < 0.5f && abs(new_pos.z - center.z) < 0.5f))
		{
			frustum->pos = new_pos;
		}
	}
	LookAt(center);
}

void ComponentCamera::LookAt(float3 target)
{
	float3 dir = (target - frustum->pos).Normalized();
	float3x3 look = float3x3::LookAt(frustum->front, dir, frustum->up, float3::unitY);
	frustum->front = look.Mul(frustum->front).Normalized();
	frustum->up = look.Mul(frustum->up).Normalized();
}

void ComponentCamera::Resize(int width, int height)
{
	frustum->horizontalFov = 2.f * atanf(tanf(frustum->verticalFov * 0.5f) * ((float)width / (float)height));
	CreateFrameBuffer(); //We recreate framebuffer with new window size
}

void ComponentCamera::Update() 
{
	if (gameobject == nullptr) return;
	if (gameobject->transform == nullptr) return;

	float4x4 transform = gameobject->GetGlobalTransform();
	frustum->pos = transform.TranslatePart();
	frustum->front = transform.RotatePart().Mul(float3::unitZ).Normalized();
	frustum->up = transform.RotatePart().Mul(float3::unitY).Normalized();
}
void ComponentCamera::DrawProperties()
{
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat("Znear", (float*)&frustum->nearPlaneDistance, 0.1f, 0.01f, 1000.f);
		ImGui::DragFloat("Zfar", (float*)&frustum->farPlaneDistance, 0.5f, 1.f, 1000.f);
		float degFov = math::RadToDeg(frustum->verticalFov);
		if (ImGui::SliderFloat("FOV", &degFov, 40, 120))
		{
			frustum->verticalFov = math::DegToRad(degFov);
			frustum->horizontalFov = 2.f * atanf(tanf(frustum->verticalFov*0.5f)*App->window->width / App->window->height);
		}

		ImGui::Separator();
	}
}

void ComponentCamera::Save(JSON_value * value) const
{
	Component::Save(value);
	value->AddFloat("MovementSpeed", movementSpeed);
	value->AddFloat("RotationSpeed", rotationSpeed);
	value->AddFloat("ZoomSpeed", zoomSpeed);
	value->AddFloat("Znear", frustum->nearPlaneDistance);
	value->AddFloat("Zfar", frustum->farPlaneDistance);
	value->AddFloat("vFOV", frustum->verticalFov);
	value->AddFloat("hFOV", frustum->horizontalFov);
	value->AddFloat3("Position", frustum->pos);
	value->AddFloat3("Front", frustum->front);
	value->AddFloat3("Up", frustum->up);
}

void ComponentCamera::Load(const JSON_value & value)
{
	Component::Load(value);
	movementSpeed = value.GetFloat("MovementSpeed");
	rotationSpeed = value.GetFloat("RotationSpeed");
	zoomSpeed = value.GetFloat("ZoomSpeed");
	frustum->nearPlaneDistance = value.GetFloat("Znear");
	frustum->farPlaneDistance = value.GetFloat("Zfar");
	frustum->verticalFov = value.GetFloat("vFOV");
	frustum->horizontalFov = value.GetFloat("hFOV");
	frustum->pos = value.GetFloat3("Position");
	frustum->front = value.GetFloat3("Front");
	frustum->up = value.GetFloat3("Up");
}


bool ComponentCamera::CleanUp()
{
	if (FBO != 0)
	{
		glDeleteFramebuffers(1, &FBO);
	}
	if (RBO != 0)
	{
		glDeleteRenderbuffers(1, &RBO);
	}
	if (camTexture != 0)
	{
		glDeleteTextures(1, &camTexture);
	}
	return true;
}

float4x4 ComponentCamera::GetViewMatrix() const
{
	float4x4 view = frustum->ViewMatrix();
	return view.Transposed();
}

float4x4 ComponentCamera::GetProjectionMatrix() const
{
	return frustum->ProjectionMatrix().Transposed();
}

void ComponentCamera::CreateFrameBuffer()
{
	CleanUp(); //Delete old FBO,RBO and texture

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &camTexture);
	glBindTexture(GL_TEXTURE_2D, camTexture);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGB, App->window->width, App->window->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, camTexture, 0
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


