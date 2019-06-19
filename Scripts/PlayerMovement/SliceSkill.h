#ifndef  __SliceSkill_h__
#define  __SliceSkill_h__

#include "MeleeSkill.h"

class SliceSkill :
	public MeleeSkill
{
public:
	SliceSkill(PlayerMovement* PM, ComponentBoxTrigger* attackBox);
	~SliceSkill();

	void Start() override;
	//void Update() override;

	void Expose(ImGuiContext* context) override;
	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

protected:
	void UseSkill() override;
	void Reset() override;
};

#endif