#include "EnemyStateChase.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "BasicEnemyAIScript.h"
#include "EnemyControllerScript.h"

#include "debugdraw.h"

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
	// Move towards the player
	enemy->enemyController->Move(enemy->chaseSpeed, refreshTime, enemy->enemyController->GetPlayerPosition(), enemyPath);

	// Check collision
	if (enemy->enemyController->IsCollidingWithPlayer())
	{
		// Player intersected, change to attack
		enemy->currentState = (EnemyState*)enemy->attack;
	}
	else
	{
		// Check if player is too far
		float distanceToPlayer = enemy->enemyController->GetDistanceToPlayer2D();
		if (distanceToPlayer > enemy->returnDistance)
		{
			// Return to start position
			enemy->currentState = (EnemyState*)enemy->returnToStart;
		}
	}

	if (enemy->drawDebug)
	{
		math::float3 playerPos = enemy->enemyController->GetPlayerPosition();
		dd::point(playerPos, dd::colors::Purple, 10.0f);
		dd::line(enemy->enemyController->GetPosition(), playerPos, dd::colors::Purple);
	}
}
