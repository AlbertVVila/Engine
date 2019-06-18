#ifndef  __SliceSkill_h__
#define  __SliceSkill_h__

#include "MeleeSkill.h"

#ifdef SliceSkill_EXPORTS
#define SliceSkill_API __declspec(dllexport)
#else
#define SliceSkill_API __declspec(dllimport)
#endif

class JSON_value;

class SliceSkill_API SliceSkill : public MeleeSkill
{
public:
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context) override;
	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	void UseSkill();


};

#endif __SliceSkill_h__
