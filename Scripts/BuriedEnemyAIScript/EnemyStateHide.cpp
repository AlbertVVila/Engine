#include "EnemyStateHide.h"

#include "EnemyControllerScript.h"
#include "BuriedEnemyAIScript.h"

EnemyStateHide::EnemyStateHide(BuriedEnemyAIScript* AIScript)
{
	enemy = AIScript;
}


EnemyStateHide::~EnemyStateHide()
{
}

void EnemyStateHide::HandleIA()
{
	if (timer > duration)
	{
		enemy->currentState = (EnemyState*)enemy->hidden;
	}
}

void EnemyStateHide::Update()
{

}
