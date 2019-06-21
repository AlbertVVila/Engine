#include "CircularAttackSkill.h"

#include "PlayerMovement.h"

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

void CircularAttackSkill::UseSkill()
{
	if (player->attackBoxTrigger != nullptr && !player->attackBoxTrigger->enabled && timer < player->currentState->duration)
	{
		// Update hitbox
		boxPosition = player->transform->up * 100.f;
		player->attackBoxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z);
	}
}

