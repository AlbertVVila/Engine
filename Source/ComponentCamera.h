#ifndef __ComponentCamera_h__
#define __ComponentCamera_h__

#include "Component.h"
#include "Math/float3.h"
class ComponentCamera :
	public Component
{
public:
	ComponentCamera();
	ComponentCamera(GameObject* gameobject);
	~ComponentCamera();

	ComponentCamera* Clone();
	void Center();
	void Move();
	void Rotate();
	void Zoom();
	void Orbit();

private:
	void ComputeEulerAngles();

public:
	float3 cameraPos = float3(0, 5, 10);
	float3 cameraFront = float3(0, 0, -1);
	float movementSpeed = 5.0f;
	float rotationSpeed = 200.f;
	float zoomSpeed = 0.1f;

private:

	float pitch = 0;
	float yaw = -90;
	float radius = 0;

	float orbitX = 0;
	float orbitY = 0;
};

#endif __ComponentCamera_h__
