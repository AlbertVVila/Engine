#include "EnemyStatePatrol.h"

#include "BasicEnemyAIScript.h"

#include "Math/float3.h"

EnemyStatePatrol::EnemyStatePatrol(BasicEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "Patrol";
}

EnemyStatePatrol::~EnemyStatePatrol()
{
}

void EnemyStatePatrol::Update()
{
	math::float3 enemyCurrentPosition = enemy->GetPosition();
	math::float3 playerCurrentPosition = enemy->GetPlayerPosition();
	float distance = enemy->GetDistanceTo2D(playerCurrentPosition);

	if (distance < enemy->activationDistance)
		enemy->currentState = (EnemyState*)enemy->chase;
}
