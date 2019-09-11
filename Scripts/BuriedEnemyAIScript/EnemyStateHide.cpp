#include "EnemyStateHide.h"

#include "EnemyControllerScript.h"
#include "BuriedEnemyAIScript.h"

#include "GameObject.h"
#include "ComponentRenderer.h"
#include "ComponentBoxTrigger.h"

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
	enemy->enemyController->GetMainRenderer()->Enable(false);
	enemy->candleGO->SetActive(false);
}

void EnemyStateHide::Enter()
{
	enemy->enemyController->hpBoxTrigger->Enable(false);
}
