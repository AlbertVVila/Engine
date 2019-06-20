#include "ChainAttackSkill.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"

#include "PlayerMovement.h"

ChainAttackSkill::ChainAttackSkill(PlayerMovement* PM, ComponentBoxTrigger* attackBox) :
	MeleeSkill(PM, attackBox)
{
	minTime = 0.40f;
	maxTime = 0.65f;
}

ChainAttackSkill::~ChainAttackSkill()
{
}

void ChainAttackSkill::Start()
{
	player->gameobject->transform->LookAtMouse();
	boxSize = math::float3(150.f, 100.f, 100.f);
	attackBoxTrigger->Enable(true);
	attackBoxTrigger->SetBoxSize(boxSize);
}

void ChainAttackSkill::UseSkill()
{
	if (attackBoxTrigger != nullptr)
	{
		//Create the hitbox
		boxPosition = player->transform->up *100.f; //this front stuff isnt working well when rotating the chicken
		attackBoxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
		hitboxCreated = true;
	}

	CheckInput();
}

void ChainAttackSkill::Reset()
{
	MeleeSkill::Reset();

	if (attackBoxTrigger != nullptr)
	{
		attackBoxTrigger->Enable(false);
		hitboxCreated = false;
	}

	// Play animation
}

void ChainAttackSkill::CheckInput()
{
	if (timer > duration * minTime)
	{
		if (player->IsAtacking())
		{
			if ((unsigned)attack < 2u)
			{
				attack = (attackNumber)((unsigned)attack + 1u);
				// Go to next attack
				Reset();
				player->currentSkill = player->chain;
				Start();
			}
			else
				attack = attackNumber::FIRST;
			return;
		}
	}

	if (timer > duration * maxTime) //CAN SWITCH?
	{

		if (player->IsUsingSkill())
		{
			//player->currentState = (PlayerState*)player->attack;
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