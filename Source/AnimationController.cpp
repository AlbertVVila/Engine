#include "AnimationController.h"



AnimationController::AnimationController()
{
}


AnimationController::~AnimationController()
{
}



math::float3 AnimationController::InterpolateFloat3(const math::float3 first, const math::float3 second, float lambda) const
{
	return first * (1.0f - lambda) + second * lambda;
}

math::Quat AnimationController::InterpolateQuat(const math::Quat first, const math::Quat second, float lambda) const
{
	Quat result;
	float dot = first.Dot(second);

	if (dot >= 0.0f) // Interpolate through the shortest path
	{
		result.x = first.x*(1.0f - lambda) + second.x*lambda;
		result.y = first.y*(1.0f - lambda) + second.y*lambda;
		result.z = first.z*(1.0f - lambda) + second.z*lambda;
		result.w = first.w*(1.0f - lambda) + second.w*lambda;
	}
	else
	{
		result.x = first.x*(1.0f - lambda) - second.x*lambda;
		result.y = first.y*(1.0f - lambda) - second.y*lambda;
		result.z = first.z*(1.0f - lambda) - second.z*lambda;
		result.w = first.w*(1.0f - lambda) - second.w*lambda;
	}

	result.Normalize();

	return result;
}