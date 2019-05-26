#include "EnemyStateReturnToStart.h"

#include "HiddenEnemyAIScript.h"

EnemyStateReturnToStart::EnemyStateReturnToStart(HiddenEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "ReturnToStart";
}

EnemyStateReturnToStart::~EnemyStateReturnToStart()
{
}

void EnemyStateReturnToStart::Update()
{
	// Look at start position and move towards
	enemy->LookAt2D(enemy->startPosition);
	enemy->MoveTowards(enemy->returnSpeed);

	// Check distance to player
	math::float3 enemyCurrentPosition = enemy->GetPosition();
	math::float3 playerCurrentPosition = enemy->GetPlayerPosition();
	float distanceToPlayer = enemy->GetDistanceTo2D(playerCurrentPosition);

	if (distanceToPlayer < enemy->activationDistance)
	{
		enemy->currentState = (EnemyState*)enemy->chase;
	}
	else 
	{
		//Check distance to start position
		float distanceToStartPosition = enemy->GetDistanceTo2D(enemy->startPosition);
		if (distanceToStartPosition < 1.5f)
			enemy->currentState = (EnemyState*)enemy->hide;
	}
}
