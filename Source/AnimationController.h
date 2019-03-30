#ifndef __ANIMATIONCONTROLLER_H_
#define __ANIMATIONCONTROLLER_H_

#include <vector>
#include "Math/float3.h"
#include "Math/Quat.h"

class ResourceAnimation;

class AnimationController
{
public:

	struct Instance
	{
		ResourceAnimation* anim = nullptr;
		float time = 0.0f;
		bool loop = true;
		float speed = 1.0f;

		Instance* next = nullptr;
		unsigned fadeDuration = 0u;
		unsigned fadeTime = 0u;
	};

	Instance* current = nullptr;


	AnimationController();
	~AnimationController();

	void Play(ResourceAnimation* anim, bool loop, unsigned fadeTime);
	void Update(float dt);
	void UpdateInstance(Instance* ins, float dt);
	void ReleaseInstance(Instance* ins);

public:

	bool GetTransformInstance(Instance* instance, unsigned channelIndex, math::float3& position, math::Quat& rotation);

	bool GetTransform(unsigned channelIndex, math::float3& position, math::Quat& rotation);

	math::float3 InterpolateFloat3(const math::float3& first, const math::float3& second, float lambda) const;

	math::Quat InterpolateQuat(const math::Quat& first, const math::Quat& second, float lambda) const;
};

#endif // __ANIMATIONCONTROLLER_H_