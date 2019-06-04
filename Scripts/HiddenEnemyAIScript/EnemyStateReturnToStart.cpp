#include "EnemyStateReturnToStart.h"

#include "HiddenEnemyAIScript.h"
#include "EnemyControllerScript.h"

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
	//enemy->enemyController->LookAt2D(enemy->startPosition);
	//enemy->enemyController->MoveTowards(enemy->returnSpeed);
	enemy->enemyController->Move(enemy->chaseSpeed, refreshTime, enemy->startPosition, enemyPath);

	// Check distance to player
	math::float3 enemyCurrentPosition = enemy->enemyController->GetPosition();
	math::float3 playerCurrentPosition = enemy->enemyController->GetPlayerPosition();
	float distanceToPlayer = enemy->enemyController->GetDistanceTo2D(playerCurrentPosition);

	if (distanceToPlayer < enemy->activationDistance)
	{
		enemy->currentState = (EnemyState*)enemy->chase;
	}
	else 
	{
		//Check distance to start position
		float distanceToStartPosition = enemy->enemyController->GetDistanceTo2D(enemy->startPosition);
		if (distanceToStartPosition < 1.5f)
			enemy->currentState = (EnemyState*)enemy->hide;
	}
}
