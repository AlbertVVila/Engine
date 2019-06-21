#include "CircularAttackSkill.h"

#include "PlayerMovement.h"


#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"

#include "PlayerState.h"

CircularAttackSkill::CircularAttackSkill(PlayerMovement* PM, const char* trigger, ComponentBoxTrigger* attackBox) :
	MeleeSkill(PM, trigger, attackBox)
{
}

CircularAttackSkill::~CircularAttackSkill()
{
}

void CircularAttackSkill::Start()
{
	// Create the hitbox
	boxSize = math::float3(250.f, 250.f, 250.f);

	// Set delay for hit
	hitDelay = 0.3f;
}

void CircularAttackSkill::Update()
{
	BasicSkill::Update();

	// Check when is time to enable the hitbox
	if (!atatckStarted && !attackBoxTrigger->enabled && timer > hitDelay)
	{
		spinTimer = 0.0f;
		attackBoxTrigger->Enable(true);
		attackBoxTrigger->SetBoxSize(boxSize);
		atatckStarted = true;
	}
}

void CircularAttackSkill::UseSkill()
{
	if (player->attackBoxTrigger != nullptr && !player->attackBoxTrigger->enabled && timer < player->currentState->duration)
	{
		// Update hitbox
		player->attackBoxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z);
	}

	// Check spin state
	if (player->attackBoxTrigger != nullptr && player->attackBoxTrigger->enabled)
	{
		// Full spin performed?
		if (spinTimer > fullSpinTime)
		{
			attackBoxTrigger->Enable(false);
		}
		else
		{
			spinTimer += player->App->time->gameDeltaTime;
		}
	}
	else if(atatckStarted)
	{
		// Go for next spin
		spinTimer = 0.0f;
		attackBoxTrigger->Enable(true);
	}
}

void CircularAttackSkill::Reset()
{
	spinTimer = 0.0f;
	atatckStarted = false;
	MeleeSkill::Reset();
}

