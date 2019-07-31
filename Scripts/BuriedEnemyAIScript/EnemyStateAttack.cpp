#include "EnemyStateAttack.h"

#include "BuriedEnemyAIScript.h"
#include "EnemyControllerScript.h"

EnemyStateAttack::EnemyStateAttack(BuriedEnemyAIScript* AIScript)
{
	enemy = AIScript;
	trigger = "Attack";
}


EnemyStateAttack::~EnemyStateAttack()
{
}

void EnemyStateAttack::HandleIA()
{
	if (timer > duration)
	{
		// End attack: Enter cooldown state
		timer = 0.f;
		enemy->currentState = (EnemyState*)enemy->cooldown;
	}
}

void EnemyStateAttack::Update()
{


}