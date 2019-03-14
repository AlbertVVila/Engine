#ifndef __ANIMATIONCONTROLLER_H_
#define __ANIMATIONCONTROLLER_H_

#include <vector>
#include "Math/float3.h"
#include "Math/Quat.h"

class Animation;

class AnimationController
{
public:

	struct Instance
	{
		unsigned clipUID = 0u;
		unsigned time = 0u;
		bool loop = true;
		float speed = 1.0f;

		Instance* next = nullptr;
		unsigned fadeDuration = 0u;
		unsigned fadeTime = 0u;
	};

	Instance* current = nullptr;


	AnimationController();
	~AnimationController();

	void Update(float dt);
	void UpdateInstance(Instance* ins, float dt);

public:
	void SetTransform();



	math::float3 InterpolateFloat3(const math::float3 first, const math::float3 second, float lambda) const;

	math::Quat InterpolateQuat(const math::Quat first, const math::Quat second, float lambda) const;
};

#endif // __ANIMATIONCONTROLLER_H_