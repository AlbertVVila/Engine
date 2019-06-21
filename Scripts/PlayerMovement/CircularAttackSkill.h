#ifndef  __CircularAttackSkill_h__
#define  __CircularAttackSkill_h__

#include "MeleeSkill.h"
class CircularAttackSkill :
	public MeleeSkill
{
public:
	CircularAttackSkill(PlayerMovement* PM, const char* trigger, ComponentBoxTrigger* attackBox);
	~CircularAttackSkill();

	void Start() override;

protected:
	void UseSkill() override;

private:
	float timeBetweenSpins = 1.0f;
};

#endif __CircularAttackSkill_h__