#ifndef __ANIMATIONCONTROLLER_H_
#define __ANIMATIONCONTROLLER_H_

#include <vector>
#include "Math/float3.h"
#include "Math/Quat.h"

class AnimationController
{
public:
	AnimationController();
	~AnimationController();

public:
	void SetTransform();



	math::float3 InterpolateFloat3(const math::float3 first, const math::float3 second, float lambda) const;

	math::Quat InterpolateQuat(const math::Quat first, const math::Quat second, float lambda) const;
};

#endif // __ANIMATIONCONTROLLER_H_