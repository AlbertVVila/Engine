#include "EnemyStateMultiply.h"
#include "BasicEnemyAIScript/BasicEnemyAIScript.h"
#include "EnemyControllerScript/EnemyControllerScript.h"

#include "Application.h"
#include "ModuleScene.h"

#define ENEMY_PREFAB "TestHouse"

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
		enemy->App->scene->Spawn(ENEMY_PREFAB);//one at his right and another at his left facing player
		//an option could be that they are already there and you only enable them
	}
	//Then in both cases go to chase state
	enemy->currentState = (EnemyState*)enemy->chase;
}
