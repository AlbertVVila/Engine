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
}

EnemyStateAttack::~EnemyStateAttack()
{
}

void EnemyStateAttack::Enter()
{
	projectileShooted = false;
	duration = duration * 3;
}

void EnemyStateAttack::HandleIA()
{
	if (timer > duration && projectileShooted)
	{
		// End attack: Enter cooldown state
		timer = 0.f;
		enemy->currentState = (EnemyState*)enemy->cooldown;
	}
}

void EnemyStateAttack::Update()
{
	if (!projectileShooted)
	{
		// Delay attack
		if (timer > enemy->projectileDelay1)
		{
			// Reset projectile position
			enemy->projectile1->transform->SetPosition(enemy->enemyController->GetPosition());
			enemy->projectile1->transform->SetRotation(enemy->enemyController->GetRotation());

			enemy->projectile1->SetActive(true);
		}
		if (timer > enemy->projectileDelay2)
		{
			// Reset projectile position
			enemy->projectile2->transform->SetPosition(enemy->enemyController->GetPosition());
			enemy->projectile2->transform->SetRotation(enemy->enemyController->GetRotation());

			enemy->projectile2->SetActive(true);
		}
		if (timer > enemy->projectileDelay3)
		{
			// Reset projectile position
			enemy->projectile3->transform->SetPosition(enemy->enemyController->GetPosition());
			enemy->projectile3->transform->SetRotation(enemy->enemyController->GetRotation());

			enemy->projectile3->SetActive(true);
			projectileShooted = true;
		}
	}

	// Keep looking at player
	math::float3 playerPosition = enemy->enemyController->GetPlayerPosition();
	enemy->enemyController->LookAt2D(playerPosition);


}