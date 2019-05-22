#include "PlayerStateSecondAttack.h"

#include "PlayerMovement.h"
#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"

#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"

PlayerStateSecondAttack::PlayerStateSecondAttack(PlayerMovement* PM)
{
	player = PM;
	trigger = "SecondAttack";
	boxSize = math::float3(150.f, 100.f, 100.f);
	minTime = 0.6f;
	maxTime = 0.8f;
}


PlayerStateSecondAttack::~PlayerStateSecondAttack()
{
}

void PlayerStateSecondAttack::Update()
{
	if (player->boxTrigger != nullptr && !hitboxCreated && timer > player->secondAttackDuration * minTime && timer < player->secondAttackDuration * maxTime)
	{
		//Create the hitbox
		player->boxTrigger->SetBoxSize(boxSize);
		boxPosition = player->transform->up *100.f; //this front stuff isnt working well when rotating the chicken
		player->boxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
		hitboxCreated = true;
	}
	if (player->boxTrigger != nullptr && hitboxCreated && timer > player->secondAttackDuration* maxTime)
	{
		player->boxTrigger->SetBoxSize(1, 1, 1);
		hitboxCreated = false;
	}
	/*player->pathIndex = 0;
	player->path.clear();
	math::float3 attackPosition;
	if (player->Appl->scene->Intersects(attackPosition, "floor"))
	{
		player->gameobject->transform->LookAt(attackPosition);
	}*/
}

void PlayerStateSecondAttack::CheckInput()
{

	if (timer > player->secondAttackDuration * 0.95)
	{
		if (player->IsAtacking())
		{
			player->currentState = (PlayerState*)player->thirdAttack;
		}
	}
	if (timer > player->secondAttackDuration * 1.5) // can switch??¿?¿?
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
