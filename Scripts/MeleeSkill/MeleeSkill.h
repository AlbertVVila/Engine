#ifndef  __MeleeSkill_h__
#define  __MeleeSkill_h__

#include "GenericSkill.h"

#include "Math/float3.h"

#ifdef MeleeSkill_EXPORTS
#define MeleeSkill_API __declspec(dllexport)
#else
#define MeleeSkill_API __declspec(dllimport)
#endif

class PlayerMovement;

class MeleeSkill_API MeleeSkill : public GenericSkill
{
public:
	MeleeSkill(PlayerMovement* PM);
	virtual ~MeleeSkill();

	void Expose(ImGuiContext* context) override;
	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

protected:
	math::float3 boxSize = math::float3::zero;
	math::float3 boxPosition = math::float3::zero;

	bool hitboxCreated = false;
};

#endif __MeleeSkill_h__
