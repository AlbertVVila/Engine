#ifndef __ModuleCamera_h__
#define __ModuleCamera_h__

#include "Module.h"
#include "MathGeoLib.h"

class ModuleCamera : public Module
{
public:
    ModuleCamera();
    ~ModuleCamera();

	bool            Init    ();
	update_status   Update  ();
	bool            CleanUp ();

	void ProcessInput();

	void Move();
	void Rotate();
	void Zoom();
	void ComputeEulerAngles();


	float3 cameraPos = float3(0, 5, 20);
	float3 cameraFront = float3(0, 0, -1);
	float3 cameraUp = float3(0, 1, 0);

private:

	float movementSpeed = 4.0f;
	float rotationSpeed = 50.f;
	float pitch = 0;
	float yaw = -90;
};

#endif /* __ModuleCamera_h__ */
