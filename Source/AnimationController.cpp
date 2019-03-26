#include "Application.h"


#include "AnimationController.h"

#include "ModuleResourceManager.h"

#include "Animation.h"
#include "Brofiler.h"

AnimationController::AnimationController()
{
}


AnimationController::~AnimationController()
{
}

void AnimationController::Play(Animation* anim, bool loop, unsigned fadeTime)
{
	Instance* newInstance = new Instance;
	newInstance->anim = anim;
	newInstance->loop = loop;
	newInstance->fadeDuration = fadeTime;
	newInstance->next = current;
	current = newInstance;
}

void AnimationController::Update(float dt)
{
	PROFILE;
	if (current != nullptr)
	{
		UpdateInstance(current, dt);
	}
}

void AnimationController::UpdateInstance(Instance* instance, float dt)
{
	Animation* anim = instance->anim;

	if (anim != nullptr && anim->durationInSeconds > 0)
	{
		float trueDt = dt * instance->speed;

		if (trueDt > 0.0f)
		{
			float timeRemainingA = anim->durationInSeconds - instance->time;
			if (trueDt <= timeRemainingA)
			{
				instance->time += trueDt;
			}
			else if (instance->loop)
			{
				instance->time = trueDt - timeRemainingA;
			}
			else
			{
				instance->time = anim->durationInSeconds;
			}
		}
		else
		{
			float timeRemainingA = - (instance->time);
			if (trueDt >= timeRemainingA)
			{
				instance->time += trueDt;
			}
			else if (instance->loop)
			{
				instance->time = anim->durationInSeconds - timeRemainingA + trueDt;
			}
			else
			{
				instance->time = 0.0f;
			}

		}
	}

	if (instance->next != nullptr)
	{
		unsigned timeRemainingB = instance->fadeDuration - instance->fadeTime;
		if (dt <= timeRemainingB)
		{
			instance->fadeTime += dt;
			UpdateInstance(instance->next, dt);
		}
		else
		{
			ReleaseInstance(instance->next);
			instance->next = nullptr;
			instance->fadeTime = instance->fadeDuration = 0;
		}
	}
}

void AnimationController::ReleaseInstance(Instance* instance)
{
	do
	{
		Instance* next = instance->next;
		delete instance;
		instance = next;
	} while (instance != nullptr);
}

bool AnimationController::GetTransform(unsigned channelIndex, math::float3& position, math::Quat& rotation)
{
	if (current != nullptr)
	{
		return GetTransformInstance(current, channelIndex, position, rotation);
	}
	else
		return false;
}

bool AnimationController::GetTransformInstance(Instance* instance, unsigned channelIndex, math::float3& position, math::Quat& rotation)
{
	Animation* anim = instance->anim;

	if (anim != nullptr)
	{
		if (channelIndex != 999u)
		{
			assert(instance->time <= anim->durationInSeconds);
		
			//which key frame are we on? This will always return an enter, how?

			float positionKey = float(instance->time*(anim->GetNumPositions(channelIndex) - 1)) / float(anim->durationInSeconds);
			float rotationKey = float(instance->time*(anim->GetNumRotations(channelIndex) - 1)) / float(anim->durationInSeconds);

			unsigned positionIndex = unsigned(positionKey);
			unsigned rotationIndex = unsigned(rotationKey);

			float positionLambda = positionKey - float(positionIndex);
			float rotationLambda = rotationKey - float(rotationIndex);

			if (positionLambda > 0.0f)
			{
				position = InterpolateFloat3(anim->GetPosition(channelIndex, positionIndex), anim->GetPosition(channelIndex, positionIndex + 1), positionLambda);
			}
			else
			{
				position = anim->GetPosition(channelIndex, positionIndex);
			}
			if (rotationLambda > 0.0f)
			{
				rotation = InterpolateQuat(anim->GetRotation(channelIndex, rotationIndex), anim->GetRotation(channelIndex, rotationIndex + 1), rotationLambda);
			}
			else
			{
				rotation = anim->GetRotation(channelIndex, positionIndex);
			}
			if (instance->next != nullptr)
			{
				assert(instance->fadeDuration > 0.0f);

				math::float3 nextPosition = math::float3::zero;
				math::Quat nextRotation = math::Quat::identity;

				if (GetTransformInstance(instance->next, channelIndex, nextPosition, nextRotation))
				{
					float blend_lambda = float(instance->fadeTime) / float(instance->fadeDuration);

					position = InterpolateFloat3(nextPosition, position, blend_lambda);
					rotation = InterpolateQuat(nextRotation, rotation, blend_lambda);
				}
			}
			return true;
		}
	}
	return false;
}

math::float3 AnimationController::InterpolateFloat3(const math::float3& first, const math::float3& second, float lambda) const
{
	return first * (1.0f - lambda) + second * lambda;
}

math::Quat AnimationController::InterpolateQuat(const math::Quat& first, const math::Quat& second, float lambda) const
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