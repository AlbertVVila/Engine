#ifndef __PLAYERSTATEATTACK_H_
#define __PLAYERSTATEATTACK_H_

#include "PlayerState.h"

class PlayerMovement;

class PlayerStateAttack :
	public PlayerState
{
public:
	PlayerStateAttack(PlayerMovement * PM, const char* trigger,
		math::float3 boxSize, float minTime, float maxTime);
	virtual ~PlayerStateAttack();

	void Update() override;

protected:
	float duration = 1.5f;
};

#endif __PLAYERSTATEATTACK_H_
