#ifndef  __CameraController_h__
#define  __CameraController_h__

#include "BaseScript.h"

class GameObject;

#ifdef CameraController_EXPORTS
#define CameraController_API __declspec(dllexport)
#else
#define CameraController_API __declspec(dllimport)
#endif

class CameraController_API CameraController : public Script
{
	void Expose(ImGuiContext* context) override;
	void Start() override;
	void Update() override;

private:
	GameObject* player;
	float height = 0.f;
};

#endif __CameraController_h__
