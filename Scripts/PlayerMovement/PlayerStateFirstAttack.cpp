#include "PlayerStateFirstAttack.h"
#include "PlayerMovement.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "BasicSkill.h"

PlayerStateFirstAttack::PlayerStateFirstAttack(PlayerMovement * PM, const char * trigger,
	math::float3 boxSize, float minTime, float maxTime) :
	PlayerStateAttack(PM, trigger, boxSize, minTime, maxTime)
{
	this->minTime = 0.40f;
	this->maxTime = 0.65f;
}

PlayerStateFirstAttack::~PlayerStateFirstAttack()
{
}

void PlayerStateFirstAttack::Enter()
{
	player->gameobject->transform->LookAtMouse();

}

void PlayerStateFirstAttack::CheckInput()
{
	if (player->currentSkill != nullptr)
	{
		UseSkill();
	}
	else
	{
		if (timer > duration * minTime)
		{
			if (player->IsAtacking())
			{
				player->currentState = (PlayerState*)player->secondAttack;
				return;
			}
		}

		if (timer > duration * maxTime) //CAN SWITCH?
		{

			if (player->IsUsingFirstSkill())
			{
				//player->currentState = (PlayerState*)player->dash;
				player->currentState = (PlayerState*)player->firstAttack;
				player->currentSkill = (BasicSkill*)player->playerSkills[0];
			}
			else if (player->IsUsingSecondSkill())
			{
				player->currentState = (PlayerState*)player->uppercut;
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
}

void PlayerStateFirstAttack::UseSkill()
{
	BasicSkill* previous = player->currentSkill;

	player->currentSkill->Update();

	CheckSkills(previous, player->currentSkill);
}

void PlayerStateFirstAttack::CheckSkills(BasicSkill* previous, BasicSkill* current)
{
	if (previous != current)
	{
		if (previous != nullptr)
			previous->Exit();

		if(current != nullptr)
			current->Start();
	}
}
