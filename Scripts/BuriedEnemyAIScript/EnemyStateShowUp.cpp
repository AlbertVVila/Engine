#include "EnemyStateShowUp.h"

#include "BuriedEnemyAIScript.h"
#include "EnemyControllerScript.h"

#include "GameObject.h"
#include "ComponentRenderer.h"
#include "ComponentBoxTrigger.h"

EnemyStateShowUp::EnemyStateShowUp(BuriedEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "ShowUp";
}


EnemyStateShowUp::~EnemyStateShowUp()
{
}

void EnemyStateShowUp::HandleIA()
{
	if (timer > duration)
	{
		float distanceToPlayer = enemy->enemyController->GetDistanceToPlayer2D();
		if (distanceToPlayer > enemy->disengageDistance)
		{
			enemy->currentState = (EnemyState*)enemy->returnToStart;
		}
		else if (distanceToPlayer > enemy->maxAttackRange)
		{
			enemy->currentState = (EnemyState*)enemy->chase;
		}
		else
		{
			enemy->currentState = (EnemyState*)enemy->attack;
		}
	}
}

void EnemyStateShowUp::Update()
{

}

void EnemyStateShowUp::Enter()
{
	enemy->enemyController->myRender->Enable(true);
	enemy->enemyController->hpBoxTrigger->Enable(true);
	enemy->dustParticlesGO->SetActive(true);
}

void EnemyStateShowUp::Exit()
{
	enemy->dustParticlesGO->SetActive(false);
}