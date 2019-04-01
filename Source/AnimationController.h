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
		Animation* anim = nullptr;
		float time = 0.0f;
		bool loop = true;
		float speed = 1.0f;

		Instance* next = nullptr;
		unsigned fadeDuration = 0u;
		unsigned fadeTime = 0u;

		// Clipping values
		float minTime = 0.0f;
		float maxTime = -1.0f;
	};

	Instance* current = nullptr;
	float trueFrame = 0;

	AnimationController();
	~AnimationController();

	void Play(Animation* anim, bool loop, unsigned fadeTime);
	void Update(float dt);
	void UpdateInstance(Instance* ins, float dt);
	void ReleaseInstance(Instance* ins);

	void PlayAnimation();
	void ResetClipping();

public:

	bool GetTransformInstance(Instance* instance, unsigned channelIndex, math::float3& position, math::Quat& rotation);

	bool GetTransform(unsigned channelIndex, math::float3& position, math::Quat& rotation);

	math::float3 InterpolateFloat3(const math::float3& first, const math::float3& second, float lambda) const;

	math::Quat InterpolateQuat(const math::Quat& first, const math::Quat& second, float lambda) const;
};

#endif // __ANIMATIONCONTROLLER_H_