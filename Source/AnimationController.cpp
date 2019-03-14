#include "Application.h"


#include "AnimationController.h"

#include "ModuleResourceManager.h"

#include "Animation.h"

AnimationController::AnimationController()
{
}


AnimationController::~AnimationController()
{
}

void AnimationController::Update(float dt)
{
	if (current != nullptr)
	{
		UpdateInstance(current, dt);
	}
}

void AnimationController::UpdateInstance(Instance* instance, float dt)
{
	Animation* anim = static_cast<Animation*>(App->resManager->GetAnim(instance->clipUID));

	if (anim != nullptr && anim->duration > 0)
	{
		unsigned trueDt = (unsigned)(dt * instance->speed);
		trueDt = trueDt % ((unsigned)(anim->duration));
		unsigned timeRemaining = anim->duration - instance->time;

		if (trueDt <= timeRemaining)
		{
			instance->time += trueDt;
		}
		else if (instance->loop)
		{
			instance->time = trueDt - timeRemaining;
		}
		else
		{
			instance->time = anim->duration;
		}
	}

	if (instance->next != nullptr)
	{
		unsigned timeRemaining = instance->fadeDuration - instance->fadeTime;
		if (dt <= timeRemaining)
		{
			instance->fadeTime += dt;
			UpdateInstance(instance->next, dt);
		}
		else
		{
		/*	ReleaseInstance(instance->next);*/
			
		}
	}

}

//void AnimationController::ReleaseInstance(Instance* instance)
//{
//
//}

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