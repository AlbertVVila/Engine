#include "BossStateInterPhase.h"

#include "BossBehaviourScript.h"

BossStateInterPhase::BossStateInterPhase(BossBehaviourScript* AIBoss)
{
	boss = AIBoss;
}


BossStateInterPhase::~BossStateInterPhase()
{
}

void BossStateInterPhase::HandleIA()
{
	if (duration > boss->firstInterphaseDuration)
	{
		boss->currentState = (BossState*)boss->idle;
	}
}

void BossStateInterPhase::Update()
{
}

void BossStateInterPhase::Enter()
{
	//Here we reset all bosses variables for the next phase
	boss->ResetVariables();
}

void BossStateInterPhase::Exit()
{
}
