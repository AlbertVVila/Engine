#ifndef __CHAINATTACKSKILL_H_
#define __CHAINATTACKSKILL_H_

#include "MeleeSkill.h"

enum class attackNumber{FIRST, SECOND, THIRD};

class ChainAttackSkill :
	public MeleeSkill
{
public:
	ChainAttackSkill(PlayerMovement* PM, const char* trigger, ComponentBoxTrigger* attackBox);
	~ChainAttackSkill();

	void Start() override;
	//void Update() override;

protected:
	void UseSkill() override;
	void Reset() override;

private:
	void CheckInput();

private:
	attackNumber attack = attackNumber::FIRST;

	float minTime = 0.0f;
	float maxTime = 0.0f;

};

#endif __CHAINATTACKSKILL_H_