#ifndef __ComponentCamera_h__
#define __ComponentCamera_h__

#include "Component.h"
#include "Math/float2.h"
#include "Math/float3.h"

class Frustum;
class ComponentCamera :
	public Component
{
public:
	ComponentCamera(); //TODO: copy constructor in camera
	ComponentCamera(GameObject* gameobject);
	~ComponentCamera();

	void Update() override;
	void DrawProperties() override;
	void Save(JSON_value *value) const override;
	void Load(const JSON_value &value) override;
	bool CleanUp() override;

	ComponentCamera* Clone() const;
	void Center();
	void Move(float3 dir); 
	void Rotate(float dx, float dy);
	void Zoom(float mouseWheel);
	void Orbit(float dx, float dy);
	void Resize(float width, float height);
	float4x4 GetViewMatrix();
	float4x4 GetProjectionMatrix();

private:
	void InitFrustum();
	void CreateFrameBuffer();
	void LookAt(float3 target);

public:
	math::Frustum* frustum = nullptr; 

	float movementSpeed = 5.0f;
	float rotationSpeed = 200.f;
	float zoomSpeed = 0.1f;

	unsigned camTexture = 0;
	unsigned FBO = 0;
	unsigned RBO = 0;

};

#endif __ComponentCamera_h__
