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
}

ChainAttackSkill::~ChainAttackSkill()
{
}

void ChainAttackSkill::Start()
{
	MeleeSkill::Start();
	player->gameobject->transform->LookAtMouse();

	// Create the hitbox
	boxSize = math::float3(150.f, 100.f, 100.f);

	// Set delay for hit
	hitDelay = 0.1f;
}

void ChainAttackSkill::UseSkill()
{
	if (attackBoxTrigger != nullptr)
	{
		//Create the hitbox
		boxPosition = player->transform->up *100.f; //this front stuff isnt working well when rotating the chicken
		attackBoxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
	}
}

void ChainAttackSkill::Reset()
{
	MeleeSkill::Reset();
	nextInput = NextInput::NONE;
	attack = AttackNumber::FIRST;
}

void ChainAttackSkill::CheckInput()
{
	if (player->IsMoving())
	{
		Reset();
		player->currentState = (PlayerState*)player->walk;
	}
}

void ChainAttackSkill::NextChainAttack()
{
	if (attack == AttackNumber::FIRST)
	{
		// Go to next attack
		Reset();
		player->currentSkill = player->chain;
		player->currentState = (PlayerState*)player->attack;
		Start();
		// Play next attack animation
		attack = AttackNumber::SECOND;
		player->attack->trigger = "Chain2";
	}
	else
	{
		// Reset attack chain
		Reset();
		player->currentSkill = player->chain;
		player->currentState = (PlayerState*)player->attack;
		Start();
		// Play first attack animation
		attack = AttackNumber::FIRST;
		player->attack->trigger = "Chain1";
	}

	// Send trigger
	if (player->anim != nullptr)
	{
		player->anim->SendTriggerToStateMachine(player->currentState->trigger.c_str());
	}

	player->currentState->duration = player->anim->GetDurationFromClip();
}