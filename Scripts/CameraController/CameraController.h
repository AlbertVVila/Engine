#ifndef  __CameraController_h__
#define  __CameraController_h__

#include "BaseScript.h"
#include "Math/float3.h"
#include "Algorithm/Random/LCG.h"
#include "Math/Quat.h"

class GameObject;

#ifdef CameraController_EXPORTS
#define CameraController_API __declspec(dllexport)
#else
#define CameraController_API __declspec(dllimport)
#endif

class CameraController_API CameraController : public Script
{
public:
	void Start() override;
	void Update() override;

	void Shake(float duration, float intensity = 5.0f, bool smooth = true, float fadeInTime = 0.2f, float fadeOutTime = 0.8f);

private:
	void ShakeCamera(math::float3& position);

private:
	GameObject* player;
	math::float3 offset;

	bool isShaking = false;
	float shakeDuration = 0.0f;
	float shakeIntensity = 0.0f;
	float shakeTimer = 0.0f;
	bool shakeSmooth = true;
	float shakeFadeInTime = 0.0f;
	float shakeFadeOutTime = 0.0f;
	math::float3 nextPosition;

	Quat originalRotation = Quat::identity;

	LCG rand;
};

#endif __CameraController_h__
