#include "EnemyStateDeath.h"

#include "GameObject.h"
#include "BasicEnemyAIScript.h"

EnemyStateDeath::EnemyStateDeath(BasicEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "Death";
}


EnemyStateDeath::~EnemyStateDeath()
{
}

void EnemyStateDeath::Update()
{
	enemy->gameobject->SetActive(false);
}
