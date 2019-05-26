#include "EnemyStateAttack.h"

#include "BasicEnemyAIScript.h"

EnemyStateAttack::EnemyStateAttack(BasicEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "Attack";
}

EnemyStateAttack::~EnemyStateAttack()
{
}

void EnemyStateAttack::Update()
{
	// Keep looking at player
	math::float3 playerPosition = enemy->GetPlayerPosition();
	enemy->LookAt2D(playerPosition);

	if (!enemy->IsCollidingWithPlayer())
	{
		enemy->currentState = (EnemyState*)enemy->chase;
	}
	else
	{
		// Attack creating hitbox
		// TODO: Add function to make damage to the player

		//playerScript->Damage(damage);
		auxTimer = timer;
		enemy->currentState = (EnemyState*)enemy->cooldown;

		//Create the hitbox
		/*player->boxTrigger->SetBoxSize(boxSize);
		boxPosition = player->transform->up * 100.f; //this front stuff isnt working well when rotating the chicken
		player->boxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
		hitboxCreated = true;^*/
	}
}