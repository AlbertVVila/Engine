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
		math::float3 target;
		if (player->App->navigation->FindIntersectionPoint(player->gameobject->transform->GetPosition(), target))
		{
			float distance = target.Distance(player->gameobject->transform->GetPosition());

			if (distance < range)
			{
				player->attackBoxTrigger->SetBoxPosition(target.x, target.y, target.z);
			}
			else
			{
				math::float3 direction = target - player->gameobject->transform->GetPosition();
				direction.Normalize();
				target = player->gameobject->transform->GetPosition() + direction * range;
				player->attackBoxTrigger->SetBoxPosition(target.x, target.y, target.z);
			}
		}
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
