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
}


PlayerStateUppercut::~PlayerStateUppercut()
{
}

void PlayerStateUppercut::Update()
{
	if (!hitboxCreated && timer > player->uppercutDuration * 0.4)
	{
		//Create the hitbox
		player->boxTrigger->SetBoxSize(100, 100, 100);
		hitboxCreated = true;
	}
	if (hitboxCreated && timer < player->uppercutDuration* 0.8)
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
	if (timer > player->uppercutDuration * 0.95) // can switch??¿?¿?
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
