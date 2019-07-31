#include "EnemyStateMultiply.h"
#include "BasicEnemyAIScript/BasicEnemyAIScript.h"
#include "EnemyControllerScript/EnemyControllerScript.h"

#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ComponentTransform.h"

#define ENEMY_PREFAB "BasicEnemy"

EnemyStateMultiply::EnemyStateMultiply(BasicEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "Chase"; //No need to change State Machine ATM
}

EnemyStateMultiply::~EnemyStateMultiply()
{
}

void EnemyStateMultiply::HandleIA()
{
	if (!hasMultiplied)
	{
		math::float3 position = enemy->gameobject->transform->GetGlobalPosition();
		math::float3 playerPos = enemy->enemyController->GetPlayerPosition();
		math::float3 vectorPlayer = math::float3(playerPos.x, position.y, playerPos.z) - position;

		math::float3 sideDirection = vectorPlayer.Cross(math::float3::unitY);
		enemy->App->scene->Spawn(ENEMY_PREFAB, position + sideDirection.Normalized() * 200.f, Quat(0.f, 0.f, 0.f, 1.f));
		enemy->App->scene->Spawn(ENEMY_PREFAB, position - sideDirection.Normalized() * 200.f, Quat(0.f, 0.f, 0.f, 1.f));
		//one at his right and another at his left facing player
		//an option could be that they are already there and you only enable them
	}
	//Then in both cases go to chase state
	enemy->currentState = (EnemyState*)enemy->chase;
}
