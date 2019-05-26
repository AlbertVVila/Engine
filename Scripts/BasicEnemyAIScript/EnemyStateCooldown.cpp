#include "EnemyStateCooldown.h"

#include "BasicEnemyAIScript.h"

EnemyStateCooldown::EnemyStateCooldown(BasicEnemyAIScript* AIScript)
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

	if (!enemy->IsCollidingWithPlayer())
	{
		enemy->currentState = (EnemyState*)enemy->chase;
	}
	else if (waitedTime > enemy->cooldownTime)
	{
		enemy->currentState = (EnemyState*)enemy->attack;
		auxTimer = 0.0f;
	}
}
