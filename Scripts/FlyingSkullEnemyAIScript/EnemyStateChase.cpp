#include "EnemyStateChase.h"

#include "EnemyControllerScript.h"
#include "FlyingSkullEnemyAIScript.h"

EnemyStateChase::EnemyStateChase(FlyingSkullEnemyAIScript* AIScript)
{
}


EnemyStateChase::~EnemyStateChase()
{
}

void EnemyStateChase::HandleIA()
{
	float distanceToPlayer = enemy->enemyController->GetDistanceToPlayer2D();

	if (distanceToPlayer > enemy->disengageDistance)
	{
		enemy->currentState = (EnemyState*)enemy->returnToStart;
	}
	else if (distanceToPlayer < enemy->maxAttackRange && distanceToPlayer > enemy->minAttackRange)
	{
		enemy->currentState = (EnemyState*)enemy->attack;
	}
	else if (distanceToPlayer < enemy->minAttackRange)
	{
		enemy->currentState = (EnemyState*)enemy->attack;
	}
}

void EnemyStateChase::Update()
{

}