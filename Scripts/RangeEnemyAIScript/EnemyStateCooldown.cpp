#include "EnemyStateCooldown.h"

#include "GameObject.h"

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

void EnemyStateCooldown::HandleIA()
{
	if (timer > enemy->cooldownTime)
	{
		// Check distance to player
		float distanceToPlayer = enemy->enemyController->GetDistanceToPlayer2D();
		if (distanceToPlayer > enemy->maxAttackDistance || distanceToPlayer < enemy->minAttackDistance)
		{
			// Get in position to attack
			enemy->currentState = (EnemyState*)enemy->getInPosition;
		}
		else if (distanceToPlayer > enemy->disengageDistance)
		{
			// Return to start position
			enemy->currentState = (EnemyState*)enemy->returnToStart;
		}
		else if (!enemy->projectile->isActive())
		{
			// Player in range and projectile is available, change to attack
			enemy->currentState = (EnemyState*)enemy->attack;
			auxTimer = 0.0f;
		}
	}
}

void EnemyStateCooldown::Update()
{
	
}
