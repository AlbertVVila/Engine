#include "EnemyStateCooldown.h"

#include "BasicEnemyAIScript.h"
#include "EnemyControllerScript.h"

EnemyStateCooldown::EnemyStateCooldown(BasicEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "Cooldown";
}


EnemyStateCooldown::~EnemyStateCooldown()
{
}

void EnemyStateCooldown::HandleIA()
{
	float distance = enemy->enemyController->GetDistanceToPlayer2D();

	if (distance > enemy->attackRange)
	{
		enemy->currentState = (EnemyState*)enemy->chase;
	}
	else if (waitedTime > enemy->cooldownTime)
	{
		enemy->currentState = (EnemyState*)enemy->attack;
		auxTimer = 0.0f;
	}
}

void EnemyStateCooldown::Update()
{
	waitedTime = (timer - auxTimer);

}
