#ifndef __ModuleCamera_h__
#define __ModuleCamera_h__

#include "Module.h"
#include "MathGeoLib.h"
#include "SDL.h"
class ModuleCamera : public Module
{
public:
    ModuleCamera();
    ~ModuleCamera();

	bool            Init    ();
	update_status   Update  ();
	bool            CleanUp ();

	void CameraInput();

	void Move();
	void Rotate();
	void Zoom();
	void Center();
	void ComputeEulerAngles();
	void Orbit();

	void DrawGUI();

	float3 cameraPos = float3(0, 5, 20);
	float3 cameraFront = float3(0, 0, -1);
	float3 cameraUp = float3(0, 1, 0);

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
