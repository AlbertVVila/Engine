#include "EnemyStateAttack.h"

#include "BuriedEnemyAIScript.h"
#include "EnemyControllerScript.h"
#include "ProjectileScript.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "assert.h"

EnemyStateAttack::EnemyStateAttack(BuriedEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "Attack";
}


EnemyStateAttack::~EnemyStateAttack()
{
}

void EnemyStateAttack::HandleIA()
{
	if (timer > duration)
	{
		// End attack: Enter cooldown state
		timer = 0.f;
		enemy->currentState = (EnemyState*)enemy->cooldown;
	}
}

void EnemyStateAttack::Update()
{
	//Make sure the delay makes sense
	assert(enemy->projectileDelay < duration);


	if (timer > enemy->projectileDelay && !projShot)
	{
		projShot = true;
		enemy->projectileGO->transform->SetPosition(enemy->enemyController->GetPosition());
		enemy->projectileGO->transform->SetRotation(enemy->enemyController->GetRotation());
		enemy->projectileScript->shooted = true;
		projShot = true;
		enemy->projectileGO->SetActive(true);
	}

	//enemy->enemyController->LookAt(enemy->enemyController->GetPlayerPosition());

}

void EnemyStateAttack::Enter()
{
	projShot = false;

	math::float3 playerPosition = enemy->enemyController->GetPlayerPosition();
	enemy->enemyController->LookAt2D(playerPosition);
}

void EnemyStateAttack::Exit()
{

}