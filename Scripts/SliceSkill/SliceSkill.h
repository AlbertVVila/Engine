#ifndef  __SliceSkill_h__
#define  __SliceSkill_h__

#include "MeleeSkill.h"

#ifdef SliceSkill_EXPORTS
#define SliceSkill_API __declspec(dllexport)
#else
#define SliceSkill_API __declspec(dllimport)
#endif

class JSON_value;
class PlayerMovement;

class SliceSkill_API SliceSkill : public MeleeSkill
{
public:
	SliceSkill(PlayerMovement* PM);
	virtual ~SliceSkill();

	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context) override;
	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	void UseSkill();


};

#endif __SliceSkill_h__
