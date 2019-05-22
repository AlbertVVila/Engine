#include "PlayerStateUppercut.h"

#include "PlayerMovement.h"
#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"

#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"

PlayerStateUppercut::PlayerStateUppercut(PlayerMovement* PM)
{
	player = PM;
	trigger = "Uppercut";
	boxSize = math::float3(100.f, 200.f, 100.f);
}


PlayerStateUppercut::~PlayerStateUppercut()
{
}

void PlayerStateUppercut::Update()
{
	if (player->boxTrigger != nullptr && !hitboxCreated && timer > player->uppercutDuration * minTime && timer < player->uppercutDuration * maxTime)
	{
		//Create the hitbox
		player->boxTrigger->SetBoxSize(boxSize);
		boxPosition = player->transform->up *100.f; //this front stuff isnt working well when rotating the chicken
		player->boxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
		hitboxCreated = true;
	}
	if (player->boxTrigger != nullptr && hitboxCreated && timer > player->uppercutDuration* maxTime)
	{
		player->boxTrigger->SetBoxSize(1, 1, 1);
		hitboxCreated = false;
	}
}

void PlayerStateUppercut::Enter()
{

}

void PlayerStateUppercut::Exit()
{

}

void PlayerStateUppercut::CheckInput()
{
	if (timer > player->uppercutDuration) // can switch??¿?¿?
	{
		if (player->IsAtacking())
		{
			player->currentState = (PlayerState*)player->firstAttack;
		}
		else if (player->IsUsingFirstSkill())
		{
			player->currentState = (PlayerState*)player->dash;
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
