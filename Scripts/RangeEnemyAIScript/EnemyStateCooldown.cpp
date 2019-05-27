#include "EnemyStateCooldown.h"

#include "EnemyControllerScript.h"
#include "RangeEnemyAIScript.h"

EnemyStateCooldown::EnemyStateCooldown(RangeEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "Cooldown";
}


EnemyStateCooldown::~EnemyStateCooldown()
{
}

void EnemyStateCooldown::Update()
{
	float waitedTime = (timer - auxTimer);
	if (waitedTime > enemy->cooldownTime)
	{
		// Check distance to player
		float distanceToPlayer = enemy->enemyController->GetDistanceToPlayer2D();
		if (distanceToPlayer > enemy->maxAttackDistance || distanceToPlayer < enemy->minAttackDistance)
		{
			// Player in range, change to attack
			enemy->currentState = (EnemyState*)enemy->getInPosition;
		}
		else if (distanceToPlayer > enemy->disengageDistance)
		{
			// Return to start position
			enemy->currentState = (EnemyState*)enemy->returnToStart;
		}
		else
		{
			enemy->currentState = (EnemyState*)enemy->attack;
			auxTimer = 0.0f;
		}
	}
}
