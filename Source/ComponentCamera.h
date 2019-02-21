#ifndef __ComponentCamera_h__
#define __ComponentCamera_h__

#include "Component.h"
#include "Geometry/LineSegment.h"

#define ZNEARDIST .1f
#define ZFARDIST 1000.f

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
	void Load(JSON_value *value) override;

	ComponentCamera* Clone() const;
	void Center();
	void Move(float3 dir); 
	void Rotate(float dx, float dy);
	void Zoom(float mouseWheel);
	void Orbit(float dx, float dy);
	void SetAspect(float aspect);
	void SetFOV(float fov);

	void ResetFrustum();
	void DrawFrustum(unsigned shader) const;

	float4x4 GetViewMatrix() const;
	float4x4 GetProjectionMatrix() const;
	LineSegment DrawRay(float x, float y) const;
private:
	void InitFrustum();
	void SetFrustumBuffers();
	void LookAt(float3 target);

public:
	Frustum* frustum = nullptr; 

	float movementSpeed = 5.0f;
	float rotationSpeed = 200.0f;
	float zoomSpeed = 0.1f;

	bool isMainCamera = false;
	float oldAspect = 0.f;
	bool aspectDirty = false;

private:
	unsigned frustumVAO = 0;
	unsigned frustumVBO = 0;
	unsigned frustumEBO = 0;
};

#endif __ComponentCamera_h__
