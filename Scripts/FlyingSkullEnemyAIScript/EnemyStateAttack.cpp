#include "EnemyStateAttack.h"

#include "EnemyControllerScript.h"
#include "FlyingSkullEnemyAIScript.h"


EnemyStateAttack::EnemyStateAttack(FlyingSkullEnemyAIScript* AIScript)
{
	enemy = AIScript;
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