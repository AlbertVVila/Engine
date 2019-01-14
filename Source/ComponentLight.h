#ifndef __ComponentLight_h__
#define __ComponentLight_h__

#include "Component.h"
#include "Math/float3.h"
#define LIGHTTYPES 3

enum class LightType
{
	DIRECTIONAL = 0,
	POINT,
	SPOT
};

class GameObject;

class ComponentLight :
	public Component
{
public:
	ComponentLight(GameObject* gameobject);
	ComponentLight(const ComponentLight& component);
	~ComponentLight();
	ComponentLight* Clone() const override;

	void ResetValues();

	float GetAttenuationDistance() const;

	void Update() override;
	void DrawProperties() override;
	void DrawDebugLight() const;
	void Load(const JSON_value &value) override;
	void Save(JSON_value *value) const override;

public:
	LightType lightType = LightType::POINT;

	float3 position = float3::zero; //Captured in transform
	float3 direction = float3::one;
	float3 color = float3::one;

	float3 attenuation = float3(0.1f,0.1f,0.1f);
	float inner = 20.f;
	float outer = 25.f;

};

#endif __ComponentLight_h__