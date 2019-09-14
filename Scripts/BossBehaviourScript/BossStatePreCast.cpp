#include "BossStatePreCast.h"

#include "BossBehaviourScript.h"

BossStatePreCast::BossStatePreCast(BossBehaviourScript* AIBoss)
{
	boss = AIBoss;
}


BossStatePreCast::~BossStatePreCast()
{
}

void BossStatePreCast::HandleIA()
{
	if (timer > duration)
	{
		boss->currentState = (BossState*)boss->cast;
	}
}

void BossStatePreCast::Update()
{
}

void BossStatePreCast::Enter()
{
	
	duration = 2.0f;
}
