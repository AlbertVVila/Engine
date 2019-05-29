#include "PlayerStateAttack.h"
#include "PlayerMovement.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"

PlayerStateAttack::PlayerStateAttack(PlayerMovement * PM, const char * trigger, math::float3 boxSize, float minTime, float maxTime):
	PlayerState(PM, trigger, boxSize, minTime, maxTime)
{
}

PlayerStateAttack::~PlayerStateAttack()
{
}

void PlayerStateAttack::Update()
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
}
