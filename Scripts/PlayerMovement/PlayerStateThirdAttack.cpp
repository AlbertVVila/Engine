#include "PlayerStateThirdAttack.h"

#include "PlayerMovement.h"
#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"

PlayerStateThirdAttack::PlayerStateThirdAttack(PlayerMovement * PM, const char * trigger, math::float3 boxSize, 
	float minTime, float maxTime) : PlayerStateAttack(PM, trigger, boxSize, minTime, maxTime)
{
}

PlayerStateThirdAttack::~PlayerStateThirdAttack()
{
}

void PlayerStateThirdAttack::CheckInput()
{
	if (timer > duration)
	{
		if (player->IsAtacking())
		{
			player->currentState = (PlayerState*)player->firstAttack;
		}
		else if (player->IsUsingFirstSkill())
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
