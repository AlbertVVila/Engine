#ifndef __ComponentTransform_h__
#define __ComponentTransform_h__

#include "Component.h"
#include "Math/float3.h"
#include "Math/Quat.h"
class ComponentTransform : public Component
{
public:
	ComponentTransform();
	~ComponentTransform();

private:
	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 translation = float3::zero;
};

#endif __ComponentTransform_h__

