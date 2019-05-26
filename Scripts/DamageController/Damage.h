#ifndef  __Damage_h__
#define  __Damage_h__

#include "Math/float2.h"

enum class DamageType
{
	NONE = 0,
	HEALING,
	NORMAL,
	CRITICAL,
	POLLOMETER,
	RECIEVED
};

class Damage
{

public:
	int damage;
	math::float2 initialPosition = math::float2::zero;
	math::float2 finalPosition = math::float2::zero;
	math::float2 actualPosition = math::float2::zero;
	math::float2 distanceNormalized = math::float2::zero;
	DamageType type = DamageType::NONE;
	bool show = false;
};

#endif