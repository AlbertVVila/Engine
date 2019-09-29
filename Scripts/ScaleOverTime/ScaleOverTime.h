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

	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context);

	void Serialize(JSON_value* json) const;
	void DeSerialize(JSON_value* json);

public:
	float speed = 0.1f;
	math::float3 finalScale = math::float3::one;

private:
	ComponentTransform* transform = nullptr;
	float timer = 0.0f;
};

extern "C" ScaleOverTime_API Script* CreateScript();
#endif __ScaleOverTime_h__
