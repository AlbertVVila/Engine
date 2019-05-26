#include "EnemyStateChase.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "BasicEnemyAIScript.h"

EnemyStateChase::EnemyStateChase(BasicEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "Chase";
}

EnemyStateChase::~EnemyStateChase()
{
}

void EnemyStateChase::Update()
{
	// Look at player and move towards
	enemy->gameobject->transform->LookAt(enemy->GetPlayerPosition());
	enemy->MoveTowards(enemy->chaseSpeed);

	// Check collision
	if (enemy->IsCollidingWithPlayer())
	{
		// Player intersected, change to attack
		enemy->currentState = (EnemyState*)enemy->attack;
	}
	else
	{
		// Check if player is too far
		math::float3 enemyCurrentPosition = enemy->GetPosition();
		math::float3 playerCurrentPosition = enemy->GetPlayerPosition();
		float distance = enemy->GetDistanceTo2D(playerCurrentPosition);

		if (distance > enemy->returnDistance)
		{
			// Return to start position
			enemy->currentState = (EnemyState*)enemy->returnToStart;
		}
	}
}
