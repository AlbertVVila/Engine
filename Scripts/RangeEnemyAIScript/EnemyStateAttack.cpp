#include "EnemyStateAttack.h"

#include "GameObject.h"

#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"

#include "EnemyControllerScript.h"
#include "RangeEnemyAIScript.h"

EnemyStateAttack::EnemyStateAttack(RangeEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "Attack";
	boxSize = math::float3(100.f, 50.f, 50.f);
	minTime = 0.7f;
	maxTime = 0.9f;
}

EnemyStateAttack::~EnemyStateAttack()
{
}

void EnemyStateAttack::Enter()
{
	auxTimer = timer;
}

void EnemyStateAttack::Update()
{
	// Keep looking at player
	math::float3 playerPosition = enemy->enemyController->GetPlayerPosition();
	enemy->enemyController->LookAt2D(playerPosition);

	if (timer > auxTimer + enemy->attackDuration)
	{
		// End attack: Enter cooldown state
		auxTimer = timer;
		enemy->currentState = (EnemyState*)enemy->cooldown;
	}
}