#ifndef __ComponentLight_h__
#define __ComponentLight_h__

#include "Component.h"
#include "Math/float3.h"

class GameObject;

class ComponentLight :
	public Component
{
public:
	ComponentLight(GameObject* gameobject);
	ComponentLight(const ComponentLight& component);
	~ComponentLight();
	ComponentLight* Clone() const override;

	void Update() override;
	void DrawProperties() override;

public:
	float3 position = float3::zero;
};

#endif __ComponentLight_h__