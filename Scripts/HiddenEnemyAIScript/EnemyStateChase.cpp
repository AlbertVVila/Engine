#include "EnemyStateChase.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "HiddenEnemyAIScript.h"
#include "EnemyControllerScript.h"

EnemyStateChase::EnemyStateChase(HiddenEnemyAIScript* AIScript)
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
	enemy->gameobject->transform->LookAt(enemy->enemyController->GetPlayerPosition());
	enemy->enemyController->MoveTowards(enemy->chaseSpeed);

	// Check collision
	if (enemy->enemyController->IsCollidingWithPlayer())
	{
		// Player intersected, change to attack
		enemy->currentState = (EnemyState*)enemy->attack;
	}
	else
	{
		// Check if player is too far
		math::float3 enemyCurrentPosition = enemy->enemyController->GetPosition();
		math::float3 playerCurrentPosition = enemy->enemyController->GetPlayerPosition();
		float distance = enemy->enemyController->GetDistanceTo2D(playerCurrentPosition);

		if (distance > enemy->returnDistance)
		{
			// Return to start position
			enemy->currentState = (EnemyState*)enemy->returnToStart;
		}
	}
}
