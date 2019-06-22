#include "PlayerStateIdle.h"
#include "PlayerMovement.h"

PlayerStateIdle::PlayerStateIdle(PlayerMovement * PM, const char * trigger):
	PlayerState(PM, trigger)
{
}

PlayerStateIdle::~PlayerStateIdle()
{
}

void PlayerStateIdle::Update()
{
}

void PlayerStateIdle::CheckInput()
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
