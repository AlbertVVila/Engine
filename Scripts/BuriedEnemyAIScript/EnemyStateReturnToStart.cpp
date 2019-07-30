#include "EnemyStateReturnToStart.h"

#include "BuriedEnemyAIScript.h"
#include "EnemyControllerScript.h"

EnemyStateReturnToStart::EnemyStateReturnToStart(BuriedEnemyAIScript* AIScript)
{
	enemy = AIScript;
}


EnemyStateReturnToStart::~EnemyStateReturnToStart()
{
}

void EnemyStateReturnToStart::HandleIA()
{
	float distanceToPlayer = enemy->enemyController->GetDistanceToPlayer2D();
	if (distanceToPlayer < enemy->activationDistance)
	{
		enemy->currentState = (EnemyState*)enemy->chase;
	}
}

void EnemyStateReturnToStart::Update()
{

}