#include "BossStateIdle.h"

#include "BossBehaviourScript.h"

BossStateIdle::BossStateIdle(BossBehaviourScript* AIBoss)
{
	boss = AIBoss;
}


BossStateIdle::~BossStateIdle()
{
}

void BossStateIdle::HandleIA()
{
	if (timer > (duration + baseDuration))
	{
		boss->currentState = (BossState*)boss->precast;
	}
}

void BossStateIdle::Update()
{
	//Allegedly nothing could be used to spawn prefabs maybe
}

void BossStateIdle::Enter()
{
	duration = (std::rand() % 100) / 100.f;
}
