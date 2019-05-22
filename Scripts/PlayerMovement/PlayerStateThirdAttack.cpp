#include "PlayerStateThirdAttack.h"

#include "PlayerMovement.h"
#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"

#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"

PlayerStateThirdAttack::PlayerStateThirdAttack(PlayerMovement* PM)
{
	player = PM;
	trigger = "ThirdAttack";
	boxSize = math::float3(100.f, 200.f, 100.f);
	minTime = 0.75f;
	maxTime = 0.9f;
}


PlayerStateThirdAttack::~PlayerStateThirdAttack()
{
}

void PlayerStateThirdAttack::Update()
{
	if (!hitboxCreated && timer > player->thirdAttackDuration * minTime && timer < player->thirdAttackDuration * maxTime)
	{
		//Create the hitbox
		player->boxTrigger->SetBoxSize(boxSize);
		boxPosition = player->transform->up *100.f; //this front stuff isnt working well when rotating the chicken
		player->boxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
		hitboxCreated = true;
	}
	if (hitboxCreated && timer > player->thirdAttackDuration* maxTime)
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

void PlayerStateThirdAttack::CheckInput()
{
	if (timer > player->thirdAttackDuration)
	{
		if (player->IsAtacking())
		{
			player->currentState = (PlayerState*)player->firstAttack;
		}
		else if (player->IsUsingFirstSkill())
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
