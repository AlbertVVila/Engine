#include "EnemyStateAttack.h"

#include "GameObject.h"

#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"

#include "EnemyControllerScript.h"
#include "ProjectileScript.h"
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
	projShot1 = false;
	projShot2 = false;
	projShot3 = false;

	//make the enemy stop moving
	enemy->enemyController->Move(enemy->runSpeed, refreshTime, enemy->gameobject->transform->position, enemyPath);

	duration = duration * enemy->numberOfProjectiles; //should be exposed how many shots he does... In my dreams 
													 // Update: your dream has come true
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

	// Delay attack
	if (timer > enemy->projectileDelay1 && !projShot1)
	{
		// Reset projectile position
		enemy->projectile1->transform->SetPosition(enemy->enemyController->GetPosition() + math::float3(0.f,enemy->projectileScript1->offsetHeight,0.f));
		enemy->projectile1->transform->SetRotation(enemy->enemyController->GetRotation());
		enemy->projectileScript1->shooted = true;
		projShot1 = true;
		enemy->projectile1->SetActive(true);
	}
	if (enemy->numberOfProjectiles > 1 && timer > enemy->projectileDelay2 && !projShot2)
	{
		// Reset projectile position
		enemy->projectile2->transform->SetPosition(enemy->enemyController->GetPosition() + math::float3(0.f, enemy->projectileScript2->offsetHeight, 0.f));
		enemy->projectile2->transform->SetRotation(enemy->enemyController->GetRotation());
		enemy->projectileScript2->shooted = true;
		projShot2 = true;
		enemy->projectile2->SetActive(true);
	}
	if (enemy->numberOfProjectiles > 2 && timer > enemy->projectileDelay3 && !projShot3)
	{
		// Reset projectile position
		enemy->projectile3->transform->SetPosition(enemy->enemyController->GetPosition() + math::float3(0.f, enemy->projectileScript3->offsetHeight, 0.f));
		enemy->projectile3->transform->SetRotation(enemy->enemyController->GetRotation());
		enemy->projectileScript3->shooted = true;
		projShot3 = true;
		enemy->projectile3->SetActive(true);
	}
	
	// Keep looking at player
	math::float3 playerPosition = enemy->enemyController->GetPlayerPosition();
	enemy->enemyController->LookAt2D(playerPosition);
}