#ifndef  __CameraController_h__
#define  __CameraController_h__

#include "BaseScript.h"
#include "Math/float3.h"
#include "Algorithm/Random/LCG.h"

class GameObject;

#ifdef CameraController_EXPORTS
#define CameraController_API __declspec(dllexport)
#else
#define CameraController_API __declspec(dllimport)
#endif

class CameraController_API CameraController : public Script
{
	void Start() override;
	void Update() override;

	void Shake(float duration, float intensity);

private:
	GameObject* player;
	math::float3 offset;

	bool isShaking = false;
	float shakeDuration = 0.0f;
	float shakeIntensity = 0.0f;

	LCG rand;
};

#endif __CameraController_h__
