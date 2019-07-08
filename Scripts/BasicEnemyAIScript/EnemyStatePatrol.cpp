#include "EnemyStatePatrol.h"

#include "BasicEnemyAIScript.h"
#include "EnemyControllerScript.h"

#include "Math/float3.h"

EnemyStatePatrol::EnemyStatePatrol(BasicEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "Patrol";
}

EnemyStatePatrol::~EnemyStatePatrol()
{
}

void EnemyStatePatrol::HandleIA()
{
}

void EnemyStatePatrol::Update()
{
	float distanceToPlayer = enemy->enemyController->GetDistanceToPlayer2D();

	if (distanceToPlayer < enemy->activationDistance)
		enemy->currentState = (EnemyState*)enemy->chase;
}
