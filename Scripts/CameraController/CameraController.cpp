#include "CameraController.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "imgui.h"
#include "JSON.h"
#include "Math/MathFunc.h"

#define ANGLEMULTIPLIER 0.04f

CameraController_API Script* CreateScript()
{
	CameraController* instance = new CameraController;
	return instance;
}

void CameraController::Start()
{
	player = App->scene->FindGameObjectByName("Player");
	assert(player != nullptr);
	offset = gameobject->transform->GetPosition() - player->transform->GetPosition();
}

void CameraController::Update()
{
	math::float3 newPosition = offset + player->transform->GetPosition();
	if (isShaking)
	{
		ShakeCamera(newPosition);
	}
	gameobject->transform->SetPosition(newPosition);
}

void CameraController::Shake(float duration, float intensity, float fadeInTime, float fadeOutTime)
{
	isShaking = true;
	shakeDuration = duration;
	shakeIntensity = intensity;

	shakeFadeInTime = fadeInTime;
	shakeFadeOutTime = fadeOutTime;

	originalRotation = gameobject->transform->GetRotation();
	return;
}

void CameraController::ShakeCamera(math::float3& position)
{
	shakeTimer += App->time->gameDeltaTime;
	float range = shakeIntensity;

	if (shakeTimer >= shakeDuration)
	{
		isShaking = false;
	}
	else
	{
		if (shakeTimer <= shakeFadeInTime * shakeDuration)
		{
			float decay = shakeTimer / (shakeFadeInTime * shakeDuration); //increases over time
			range *= decay;
		}
		else if (shakeTimer >= shakeFadeOutTime * shakeDuration)
		{
			float fadeOutTime = (1 - shakeFadeOutTime) * shakeDuration;
			float decay = 1 - (shakeTimer - shakeFadeOutTime * shakeDuration) / fadeOutTime; //decreases over time
			range *= decay;

		}
		position  = math::float3::RandomSphere(rand, position, range);
		float roll = ANGLEMULTIPLIER * range * rand.Float() * 2 - 1;
		gameobject->transform->SetRotation(originalRotation.Mul(Quat::RotateZ(math::DegToRad(roll))));
	}
}