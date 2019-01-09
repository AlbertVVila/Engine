#ifndef __ComponentCamera_h__
#define __ComponentCamera_h__

#include "Component.h"
#include "Geometry/LineSegment.h"

#define ZNEARDIST .1f
#define ZFARDIST 1000.f

class Frustum;
class ComponentCamera :
	public Component
{
public:
	ComponentCamera();
	ComponentCamera(GameObject* gameobject);
	ComponentCamera(const ComponentCamera &copy);
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
	void SetAspect(float aspect);
	void SetFOV(float fov);
	float4x4 GetViewMatrix() const;
	float4x4 GetProjectionMatrix() const;
	LineSegment DrawRay(float x, float y);
private:
	void InitFrustum();
	void LookAt(float3 target);

public:
	math::Frustum* frustum = nullptr; 

	float movementSpeed = 5.0f;
	float rotationSpeed = 200.f;
	float zoomSpeed = 0.1f;
};

#endif __ComponentCamera_h__
