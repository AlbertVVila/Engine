#include "EnemyStateShowUp.h"

#include "BuriedEnemyAIScript.h"
#include "EnemyControllerScript.h"

EnemyStateShowUp::EnemyStateShowUp(BuriedEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "ShowUp";
}


EnemyStateShowUp::~EnemyStateShowUp()
{
}

void EnemyStateShowUp::HandleIA()
{
	if (timer > duration)
	{
		float distanceToPlayer = enemy->enemyController->GetDistanceToPlayer2D();
		if (distanceToPlayer > enemy->disengageDistance)
		{
			enemy->currentState = (EnemyState*)enemy->returnToStart;
		}
		else if (distanceToPlayer > enemy->maxAttackRange)
		{
			enemy->currentState = (EnemyState*)enemy->chase;
		}
		else
		{
			enemy->currentState = (EnemyState*)enemy->attack;
		}
	}
}

void EnemyStateShowUp::Update()
{

}