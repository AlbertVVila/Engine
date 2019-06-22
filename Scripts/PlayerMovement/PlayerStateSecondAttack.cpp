#include "PlayerStateSecondAttack.h"

#include "PlayerMovement.h"
#include "Application.h"
#include "ModuleScene.h"

PlayerStateSecondAttack::PlayerStateSecondAttack(PlayerMovement* PM, const char* trigger, math::float3 boxSize,
	float minTime, float maxTime): PlayerStateAttack(PM, trigger, boxSize, minTime, maxTime)
{
	minTime = 0.85f;
	maxTime = 0.9f;
}

PlayerStateSecondAttack::~PlayerStateSecondAttack()
{
}

void PlayerStateSecondAttack::CheckInput()
{

	if (timer > duration * minTime)
	{
		if (player->IsAtacking())
		{
			player->currentState = (PlayerState*)player->thirdAttack;
		}
	}
	if (timer > duration * maxTime) // can switch??¿?¿?
	{
		
		if (player->IsUsingFirstSkill())
		{
			player->currentState = player->allSkills[player->activeSkills[0]]->state;
		}
		else if (player->IsUsingSecondSkill())
		{
			player->currentState = player->allSkills[player->activeSkills[1]]->state;
		}
		else if (player->IsMoving())
		{
			player->currentState = (PlayerState*)player->walk;
		}
		else
		{
			player->currentState = (PlayerState*)player->idle;
		}
	}
}
