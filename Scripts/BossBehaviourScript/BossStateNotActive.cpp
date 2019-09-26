#include "BossStateNotActive.h"

#include "BossBehaviourScript.h"

BossStateNotActive::BossStateNotActive(BossBehaviourScript* AIBoss)
{
	boss = AIBoss;
}


BossStateNotActive::~BossStateNotActive()
{
}

void BossStateNotActive::HandleIA()
{
	if (boss->distanceToPlayer < boss->activationDistance)
	{
		boss->PrepareBossFight(boss->floorBossGOs);
		boss->currentState = (BossState*)boss->summonArmy;
	}
}
