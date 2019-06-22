#include "StompSkill.h"

#include "PlayerMovement.h"

#include "Application.h"
#include "ModuleNavigation.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"

#include "PlayerState.h"

StompSkill::StompSkill(PlayerMovement* PM, const char* trigger, ComponentBoxTrigger* attackBox) : MeleeSkill(PM, trigger, attackBox)
{
}


StompSkill::~StompSkill()
{
}

void StompSkill::Start()
{
	MeleeSkill::Start();

	player->gameobject->transform->LookAtMouse();

	if (StompFX != nullptr)
	{
		StompFX->SetActive(true);
	}
	player->ResetCooldown(HUB_BUTTON_1);

	//Create the hitbox
	boxSize = math::float3(500.f, 500.f, 500.f);

	// Set delay for hit
	hitDelay = 0.8f;
}

void StompSkill::UseSkill()
{
	if (player->attackBoxTrigger != nullptr && !player->attackBoxTrigger->enabled)
	{
		// Update hitbox
		boxPosition = player->transform->up * 100.f;
		player->attackBoxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
	}
}

void StompSkill::CheckInput()
{
	if (timer > player->currentState->duration) //CAN SWITCH?
	{
		if (player->IsUsingSkill())
		{
			player->currentState = (PlayerState*)player->attack;
		}
		else if (player->IsMoving())
		{
			player->currentState = (PlayerState*)player->walk;
		}
	}
}
