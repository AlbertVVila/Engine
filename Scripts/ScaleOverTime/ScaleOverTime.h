#ifndef  __ScaleOverTime_h__
#define  __ScaleOverTime_h__

#include "BaseScript.h"

#include "Math/float3.h"

#ifdef ScaleOverTime_EXPORTS
#define ScaleOverTime_API __declspec(dllexport)
#else
#define ScaleOverTime_API __declspec(dllimport)
#endif

class ComponentTransform;

class ScaleOverTime_API ScaleOverTime : public Script
{
public:
	inline virtual ScaleOverTime* Clone() const
	{
		return new ScaleOverTime(*this);
	}

private:
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context);

	void Serialize(JSON_value* json) const;
	void DeSerialize(JSON_value* json);

public:
	float speed = 0.1f;
	math::float3 finalScale = math::float3::one;		// Scale to reach

private:
	math::float3 startScale = math::float3::one;		// Scale of the GO at the start of the script
	ComponentTransform* transform = nullptr;
	float timer = 0.0f;

	bool loop = false;									// If true once the final scale has been reached the scale goes back to start scale
	bool acceleration = true;							// If true each frame the animation will play faster (once looped it gets reseted) else speed is constant
	//bool pingPong = false;
	//math::float3 reverse = math::float3::zero;
};

extern "C" ScaleOverTime_API Script* CreateScript();
#endif __ScaleOverTime_h__
