#include "PlayerStateFirstAttack.h"

#include "PlayerMovement.h"
#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"

#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"

PlayerStateFirstAttack::PlayerStateFirstAttack(PlayerMovement* PM)
{
	player = PM;
	trigger = "FirstAttack";
	boxSize = math::float3(150.f, 100.f, 100.f);
	minTime = 0.7f;
	maxTime = 0.9f;
}

PlayerStateFirstAttack::~PlayerStateFirstAttack()
{
}

void PlayerStateFirstAttack::Update()
{
	if (player->boxTrigger != nullptr && !hitboxCreated && timer > player->firstAttackDuration * minTime && timer < player->firstAttackDuration * maxTime)
	{
		//Create the hitbox
		player->boxTrigger->SetBoxSize(boxSize);
		boxPosition = player->transform->up * 100.f; //this front stuff isnt working well when rotating the chicken
		player->boxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
		hitboxCreated = true;
	}
	if (player->boxTrigger != nullptr && hitboxCreated && timer > player->firstAttackDuration* maxTime)
	{
		player->boxTrigger->SetBoxSize(1, 1, 1);
		hitboxCreated = false;
	}
	//player->pathIndex = 0;
	//player->path.clear();
	//math::float3 attackPosition;
	//if (player->Appl->scene->Intersects(attackPosition, "floor"))
	//{
	//	player->gameobject->transform->LookAt(attackPosition);
	//}
}

void PlayerStateFirstAttack::Enter()
{
}

void PlayerStateFirstAttack::Exit()
{
}

void PlayerStateFirstAttack::CheckInput()
{
	if (timer > player->firstAttackDuration * 0.95)
	{
		if (player->IsAtacking())
		{
			player->currentState = (PlayerState*)player->secondAttack;
		}
	}
	if (timer > player->firstAttackDuration  * 1.2) //CAN SWITCH?
	{
		
		if (player->IsUsingFirstSkill())
		{
			player->currentState = (PlayerState*)player->dash;
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
