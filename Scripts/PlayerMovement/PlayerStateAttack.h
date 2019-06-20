#ifndef __PLAYERSTATEATTACK_H_
#define __PLAYERSTATEATTACK_H_

#include "PlayerState.h"

class PlayerMovement;
class BasicSkill;

class PlayerStateAttack :
	public PlayerState
{
public:
	PlayerStateAttack(PlayerMovement * PM, const char* trigger,
		math::float3 boxSize, float minTime, float maxTime);
	virtual ~PlayerStateAttack();

	void Update() override;

	void UseSkill();
	void CheckSkills(BasicSkill* previous, BasicSkill* current);
};

#endif __PLAYERSTATEATTACK_H_
