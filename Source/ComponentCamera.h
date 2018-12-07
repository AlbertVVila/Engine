#ifndef __ComponentCamera_h__
#define __ComponentCamera_h__

#include "Component.h"
#include "Math/float2.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"

class ComponentCamera :
	public Component
{
public:
	ComponentCamera();
	ComponentCamera(GameObject* gameobject);
	~ComponentCamera();

	ComponentCamera* Clone();
	void InitFrustum();
	void Center();
	void Move(); 
	void Rotate(float dx, float dy);
	void Zoom(float mouseWheel);
	void Orbit(float dx, float dy);
	void LookAt(float3 target);
	void Resize(float width, float height);
	void Update() override;
	void DrawProperties() override;
	void Save(JSON_value *value) override;

	float4x4 GetViewMatrix();
	float4x4 GetProjectionMatrix();

private:
	void CreateFrameBuffer();

public:
	float movementSpeed = 5.0f;
	float rotationSpeed = 200.f;
	float zoomSpeed = 0.1f;
	math::Frustum frustum; //TODO: better as a pointer
	unsigned int renderedTexture = 0;
	unsigned int FBO = 0;
	unsigned int RBO = 0;

};

#endif __ComponentCamera_h__
