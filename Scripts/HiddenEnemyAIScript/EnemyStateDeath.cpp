#include "EnemyStateDeath.h"

#include "GameObject.h"
#include "HiddenEnemyAIScript.h"

EnemyStateDeath::EnemyStateDeath(HiddenEnemyAIScript* AIScript)
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
