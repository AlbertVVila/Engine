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
		boss->currentState = (BossState*)boss->idle;
	}
}

void BossStatePreCast::Update()
{
}

void BossStatePreCast::Enter()
{
	/* TODO duration = skill->castDuration;*/
	duration = 2.0f;
}
