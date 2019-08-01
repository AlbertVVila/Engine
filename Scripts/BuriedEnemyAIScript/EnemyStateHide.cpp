#include "EnemyStateHide.h"

#include "EnemyControllerScript.h"
#include "BuriedEnemyAIScript.h"

#include "ComponentRenderer.h"

EnemyStateHide::EnemyStateHide(BuriedEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "Hide";
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

void EnemyStateHide::Exit()
{
	enemy->enemyController->myRender->Enable(false);
}
