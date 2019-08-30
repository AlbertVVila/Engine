#include "EnemyStateAttackSpin.h"

#include "GameObject.h"

#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"
#include "ComponentRenderer.h"

#include "ResourceMaterial.h"

#include "BasicEnemyAIScript/BasicEnemyAIScript.h"
#include "EnemyControllerScript/EnemyControllerScript.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "ModuleResourceManager.h"

#include "ComponentTrail.h"

#define DEFAULTMATERIAL "Small_Skeleton"
#define ROTATIONMATERIAL "0VolLight"

EnemyStateAttackSpin::EnemyStateAttackSpin(BasicEnemyAIScript* AIScript) : EnemyStateAttack(AIScript)
{
	trigger = "ReturnToStart";
	enemyRenderer = (ComponentRenderer*)enemy->gameobject->GetComponentInChildren(ComponentType::Renderer);
}

EnemyStateAttackSpin::~EnemyStateAttackSpin()
{
}

void EnemyStateAttackSpin::HandleIA() //Should check spin 75% prob & heatlh < 50% + timer to do it again (x seconds)
{
	//If player is in range nothing, else if player is in range chase, if enemy has attacked cooldown
	
	if (timer > duration)
	{
		float distance = enemy->enemyController->GetDistanceToPlayer2D();
		if (distance > enemy->attackRange) //if not in range chase
		{
			if (hitboxCreated)
			{
				// Disable hitbox
				enemy->enemyController->attackBoxTrigger->Enable(false);
				hitboxCreated = false;
			}
			PunchFX(false);
			enemy->currentState = (EnemyState*)enemy->chase;
		}
		else if (attacked)
		{
			if (hitboxCreated)
			{
				// Disable hitbox
				enemy->enemyController->attackBoxTrigger->Enable(false);
				hitboxCreated = false;
			}
			enemy->currentState = (EnemyState*)enemy->cooldown;
			attacked = false;
			PunchFX(false);
		}
	}

}

void EnemyStateAttackSpin::Update()
{
	// Keep looking at player
	math::float3 playerPosition = enemy->enemyController->GetPlayerPosition();
	//enemy->enemyController->LookAt2D(playerPosition);


	assert(enemy->enemyController->attackBoxTrigger != nullptr);

	Attack();
}

void EnemyStateAttackSpin::Attack() //Split into SPIN or normal ATTACK
{

	if ((lcg.Float() < 0.25 || isOnCooldown) && !spinning) //Normal attack
	{
		if (strcmp(enemyRenderer->material->GetName(),DEFAULTMATERIAL)== 0)
		{
			enemyRenderer->SetMaterial(DEFAULTMATERIAL);
			enemy->gameobject->LinkBones();
		}
		enemy->enemyController->attackBoxTrigger->Enable(true);
		enemy->enemyController->attackBoxTrigger->SetBoxSize(boxSize);
		boxPosition = enemy->gameobject->transform->up * 100.f;
		enemy->enemyController->attackBoxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
		hitboxCreated = true;
		attacked = true;
		spinTimer += enemy->App->time->gameDeltaTime;
		if (spinTimer > spinCooldown)
		{
			isOnCooldown = false;
			spinTimer = 0.0f;
		}
		PunchFX(true);
	}
	//Spin -> should add a cooldown timer
	else if(spinTimer < spinDuration)
	{
		if (!spinning)
		{
			enemyRenderer->SetMaterial(ROTATIONMATERIAL);
		}
		spinning = true;
		spinTimer += enemy->App->time->gameDeltaTime;
		enemy->gameobject->transform->Rotate(math::float3(0.f,enemy->App->time->gameDeltaTime*5.f, 0.f)); //SHOULD ROTATE WITH DELTATIME

		LOG("ROTATING");
	}
	else
	{
		if (strcmp(enemyRenderer->material->GetName(), DEFAULTMATERIAL) == 0)
		{
			enemyRenderer->SetMaterial(DEFAULTMATERIAL);
			enemy->gameobject->LinkBones();
		}
		LOG("NONE");
		spinTimer = 0.0f;
		spinning = false;
		isOnCooldown = true;
		attacked = true;
	}
	//Create the hitbox
}
