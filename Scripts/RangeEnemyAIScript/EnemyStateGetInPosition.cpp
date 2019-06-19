#include "EnemyStateGetInPosition.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "EnemyControllerScript.h"
#include "RangeEnemyAIScript.h"

#include "debugdraw.h"

EnemyStateGetInPosition::EnemyStateGetInPosition(RangeEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "GetInPosition";
}

EnemyStateGetInPosition::~EnemyStateGetInPosition()
{
}

void EnemyStateGetInPosition::Update()
{
	float distanceToPlayer = enemy->enemyController->GetDistanceToPlayer2D();
	if (distanceToPlayer > enemy->maxAttackDistance)
	{
		// Not near enough from player to attack: check if we are too far to continue following him
		if (distanceToPlayer > enemy->disengageDistance)
		{
			// Too far: Return to start position
			enemy->currentState = (EnemyState*)enemy->returnToStart;
		}
		else
		{
			// Near enough: aproach attack distance
			AproachPlayer();
		}
	}
	else if(distanceToPlayer < enemy->minAttackDistance)
	{
		// Player is too near: go 
		MoveAwayFromPlayer();
	}
	else
	{
		// Player in range: change to attack
		enemy->currentState = (EnemyState*)enemy->attack;
	}
}

void EnemyStateGetInPosition::AproachPlayer()
{
	math::float3 playerPos = enemy->enemyController->GetPlayerPosition();
	enemy->enemyController->Move(enemy->runSpeed, refreshTime, playerPos, enemyPath);

	if (enemy->drawDebug)
	{
		dd::point(playerPos, dd::colors::Purple, 10.0f);
		dd::line(enemy->enemyController->GetPosition(), playerPos, dd::colors::Purple);
	}
}

void EnemyStateGetInPosition::MoveAwayFromPlayer()
{
	float distanceToPlayer = enemy->enemyController->GetDistanceToPlayer2D();

	math::float3 playerPos = enemy->enemyController->GetPlayerPosition();
	math::float3 enemyPos = enemy->enemyController->GetPosition();

	// Get Euclidean Vector (Vector Director) 
	float x = enemyPos.x - playerPos.x;
	float z = enemyPos.z - playerPos.z;

	// Normalize
	x /= distanceToPlayer;
	z /= distanceToPlayer;

	// Get the new point which is at maxAttackDistance from player position
	math::float3 newPosition;
	newPosition.x = ((float)playerPos.x + x * (enemy->maxAttackDistance));
	newPosition.y = enemyPos.y;
	newPosition.z = ((float)playerPos.z + z * (enemy->maxAttackDistance));

	// Move to that point
	enemy->enemyController->Move(enemy->runSpeed, refreshTime, newPosition, enemyPath);

	if (enemy->drawDebug)
	{
		dd::point(newPosition, dd::colors::Purple, 10.0f);
		dd::line(enemyPos, newPosition, dd::colors::Purple);
	}
}
