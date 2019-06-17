#ifndef  __SliceSkill_h__
#define  __SliceSkill_h__

#include "BaseScript.h"

#ifdef SliceSkill_EXPORTS
#define SliceSkill_API __declspec(dllexport)
#else
#define SliceSkill_API __declspec(dllimport)
#endif

class JSON_value;

class SliceSkill_API SliceSkill : public Script
{
public:
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context) override;
	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	void UseSkill();

public:
	float speed = 1.0f;
	float duration = 1.0f;

private:
	float timer = 0.0f;
};

#endif __SliceSkill_h__
