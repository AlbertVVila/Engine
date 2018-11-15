#ifndef __ComponentTransform_h__
#define __ComponentTransform_h__

#include "Component.h"
#include "Math/float3.h"
#include "Math/Quat.h"
#include "assimp/matrix4x4.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform(GameObject* gameobject, const aiMatrix4x4 &transform = aiMatrix4x4());
	~ComponentTransform();

	void AddTransform(const aiMatrix4x4 &transform);

public:
	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::zero;
};

#endif __ComponentTransform_h__
