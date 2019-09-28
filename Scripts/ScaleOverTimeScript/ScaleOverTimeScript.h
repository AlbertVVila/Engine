#ifndef  __ScaleOverTimeScript_h__
#define  __ScaleOverTimeScript_h__

#include "BaseScript.h"

#include "Math/float3.h"

#ifdef ScaleOverTimeScript_EXPORTS
#define ScaleOverTimeScript_API __declspec(dllexport)
#else
#define ScaleOverTimeScript_API __declspec(dllimport)
#endif

class ComponentTransform;

class ScaleOverTimeScript_API ScaleOverTimeScript : public Script
{
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context);

	void Serialize(JSON_value* json) const;
	void DeSerialize(JSON_value* json);

public:
	float speed = 1.0f;
	math::float3 finalScale = math::float3::one;

private:
	ComponentTransform* transform = nullptr;
	float timer = 0.0f;
};

#endif __ScaleOverTimeScript_h__
