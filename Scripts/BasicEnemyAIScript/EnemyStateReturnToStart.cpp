#include "EnemyStateReturnToStart.h"

#include "BasicEnemyAIScript.h"
#include "EnemyControllerScript.h"

#define START_POS_OFFSET  1.5f

EnemyStateReturnToStart::EnemyStateReturnToStart(BasicEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "ReturnToStart";
}

EnemyStateReturnToStart::~EnemyStateReturnToStart()
{
}

void EnemyStateReturnToStart::HandleIA()
{
}

void EnemyStateReturnToStart::Update()
{
	// Move back to start position
	enemy->enemyController->Move(enemy->chaseSpeed, refreshTime, enemy->startPosition, enemyPath);

	// Check distance to player
	float distanceToPlayer = enemy->enemyController->GetDistanceToPlayer2D();
	if (distanceToPlayer < enemy->activationDistance)
	{
		enemy->currentState = (EnemyState*)enemy->chase;
	}
	else 
	{
		//Check distance to start position
				// TODO: Change to check that there is not a nearest point on the nav mesh to this
		float distanceToStartPosition = enemy->enemyController->GetDistanceTo2D(enemy->startPosition);
		if (distanceToStartPosition < START_POS_OFFSET)
			enemy->currentState = (EnemyState*)enemy->patrol;
	}
}
