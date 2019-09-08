#include "EnemyStateDeath.h"

#include "EnemyControllerScript.h"
#include "FlyingSkullEnemyAIScript.h"

#include "GameObject.h"

EnemyStateDeath::EnemyStateDeath(FlyingSkullEnemyAIScript* AIScript)
{
	enemy = AIScript;
}


EnemyStateDeath::~EnemyStateDeath()
{
}

void EnemyStateDeath::HandleIA()
{

}

void EnemyStateDeath::Update()
{
	if (timer > waitTime)
	{
		enemy->gameobject->SetActive(false);
	}
}