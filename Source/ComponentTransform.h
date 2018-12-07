#ifndef __ComponentTransform_h__
#define __ComponentTransform_h__

#include "Component.h"
#include "Math/float3.h"
#include "Math/Quat.h"
#include "assimp/matrix4x4.h"
#include "Math/float4x4.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform(GameObject* gameobject, const float4x4 &transform = float4x4::identity);
	ComponentTransform(const ComponentTransform& component);
	~ComponentTransform();

	Component* Clone() override;
	void AddTransform(const float4x4 &transform);
	void DrawProperties() override;
	void SetRotation(const Quat& rot);
	void RotationToEuler();
	void SetPosition(const float3& pos);

	void SetLocalToWorld(const float4x4 & parentGlobalMatrix);
	void SetWorldToLocal(const float4x4 & newparentGlobalMatrix);

	void Save(JSON_value *value) override;
	void Load(JSON_value *value) override;

public:
	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 eulerRotation = float3::zero;
	float3 scale = float3::zero;
};

#endif __ComponentTransform_h__

