#include "EnemyStateDeath.h"

#include "GameObject.h"
#include "RangeEnemyAIScript.h"

EnemyStateDeath::EnemyStateDeath(RangeEnemyAIScript* AIScript)
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
