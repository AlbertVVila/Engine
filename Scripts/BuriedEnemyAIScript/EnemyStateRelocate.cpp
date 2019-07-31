#include "EnemyStateRelocate.h"

#include "BuriedEnemyAIScript.h"
#include "EnemyControllerScript.h"

#include "Application.h"
#include "ModuleNavigation.h"

#include "Math/float3.h"
#include "Math/MathFunc.h"
#include "Geometry/Frustum.h"

EnemyStateRelocate::EnemyStateRelocate(BuriedEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "Relocate";
}


EnemyStateRelocate::~EnemyStateRelocate()
{
}

void EnemyStateRelocate::HandleIA()
{
	if (timer > duration)
	{
		enemy->currentState = (EnemyState*)enemy->showUp;
		finished = true;
		ChangePosition();
	}
}

void EnemyStateRelocate::Update()
{

}

void EnemyStateRelocate::ChangePosition()
{
	//Here we find a point far from the player, preferably in the other direction
	math::float3 initialPos = enemy->enemyController->GetPosition();
	math::float3 playerPos = enemy->enemyController->GetPlayerPosition();

	math::float3 direction = playerPos - initialPos;
	direction.Normalize();

	math::float3 newPos = playerPos + direction * enemy->teleportDistance;
	

	if (enemy->App->navigation->FindClosestPoint2D(newPos))
	{
		enemy->enemyController->SetPosition(newPos);		
	}
}
