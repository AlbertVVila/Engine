#ifndef  __MeleeSkill_h__
#define  __MeleeSkill_h__

#include "BaseSkill.h"

#include "Math/float3.h"

class PlayerMovement;
class ComponentBoxTrigger;
class ImGuiContext;
class JSON_value;

class MeleeSkill :
	public BaseSkill
{
public:
	MeleeSkill(PlayerMovement* PM, ComponentBoxTrigger* attackBox);
	virtual ~MeleeSkill();

	void Expose(ImGuiContext* context) override;
	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

protected:
	virtual void Reset();

protected:
	ComponentBoxTrigger* attackBoxTrigger = nullptr;

	math::float3 boxSize = math::float3::zero;
	math::float3 boxPosition = math::float3::zero;

	bool hitboxCreated = false;
};

#endif __MeleeSkill_h__