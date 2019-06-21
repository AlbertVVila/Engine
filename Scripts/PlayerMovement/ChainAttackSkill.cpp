#include "ChainAttackSkill.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"
#include "ComponentAnimation.h"

#include "PlayerStateAttack.h"

#include "ResourceStateMachine.h"

#include "PlayerMovement.h"

ChainAttackSkill::ChainAttackSkill(PlayerMovement* PM, const char* trigger, ComponentBoxTrigger* attackBox) :
	MeleeSkill(PM, trigger, attackBox)
{
	minTime = 0.40f;
	maxTime = 0.9f;
}

ChainAttackSkill::~ChainAttackSkill()
{
}

void ChainAttackSkill::Start()
{
	BasicSkill::Start();
	player->gameobject->transform->LookAtMouse();

	// Create the hitbox
	boxSize = math::float3(150.f, 100.f, 100.f);

	// Set delay for hit
	hitDelay = 0.6f;
}

void ChainAttackSkill::UseSkill()
{
	if (attackBoxTrigger != nullptr)
	{
		//Create the hitbox
		boxPosition = player->transform->up *100.f; //this front stuff isnt working well when rotating the chicken
		attackBoxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
	}

	CheckInput();
}

void ChainAttackSkill::CheckInput()
{
	if (timer > player->currentState->duration * minTime)
	{
		if (player->IsAtacking())
		{
			NextChainAttack();
			return;
		}
	}

	if (timer > player->currentState->duration * maxTime) //CAN SWITCH?
	{

		if (player->IsUsingSkill())
		{
			player->currentState = (PlayerState*)player->attack;
		}
		else if (player->IsMoving())
		{
			player->currentState = (PlayerState*)player->walk;
		}
		else
		{
			Reset();
		}
	}
}

void ChainAttackSkill::NextChainAttack()
{
	if (attack == attackNumber::FIRST)
	{
		attack = attackNumber::SECOND;
		// Go to next attack
		Reset();
		player->currentSkill = player->chain;
		player->currentState = (PlayerState*)player->attack;
		Start();
		// Play next attack animation
		player->attack->trigger = "Chain2";
	}
	else
	{
		attack = attackNumber::FIRST;
		// Reset attack chain
		Reset();
		player->currentSkill = player->chain;
		player->currentState = (PlayerState*)player->attack;
		Start();
		// Play first attack animation
		player->attack->trigger = "Chain1";
	}

	// Send trigger
	player->currentState->duration = player->anim->GetDurationFromClip();
	if (player->anim != nullptr)
	{
		player->anim->SendTriggerToStateMachine(player->currentState->trigger.c_str());
	}
}
