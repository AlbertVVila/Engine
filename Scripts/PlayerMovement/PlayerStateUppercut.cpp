#include "PlayerStateUppercut.h"

#include "PlayerMovement.h"
#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"

#include "ComponentTransform.h"

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

}

void PlayerStateUppercut::Enter()
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
