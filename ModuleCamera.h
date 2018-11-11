#ifndef __ModuleCamera_h__
#define __ModuleCamera_h__

#include "Module.h"
#include "Math/float3.h"
class ModuleCamera : public Module
{
public:
    ModuleCamera();
    ~ModuleCamera();

	update_status   Update  ();
	void DrawGUI();
	void Center();

	float3 cameraPos = float3(0, 5, 10);
	float3 cameraFront = float3(0, 0, -1);
	float3 cameraUp = float3(0, 1, 0);

private:

	void Move();
	void Rotate();
	void Zoom();
	void ComputeEulerAngles();
	void Orbit();

private:

	float movementSpeed = 4.0f;
	float rotationSpeed = 50.f;
	float pitch = 0;
	float yaw = -90;
	float radius = 0;

	float startAngleX = 0;
	float startAngleY = 0;
};

#endif /* __ModuleCamera_h__ */
