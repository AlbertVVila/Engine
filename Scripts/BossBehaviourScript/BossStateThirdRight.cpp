#include "BossStateThirdRight.h"

#include "BossBehaviourScript.h"

#include "EnemyControllerScript/EnemyControllerScript.h"
#include "ComponentAnimation.h"

BossStateThirdRight::BossStateThirdRight(BossBehaviourScript* AIBoss)
{
	boss = AIBoss;
	trigger = "ThirdRight";
}


BossStateThirdRight::~BossStateThirdRight()
{
}

void BossStateThirdRight::HandleIA()
{
	if (duration > timer)
	{
		boss->currentState = (BossState*)boss->thirdIdle;
	}
}

void BossStateThirdRight::Update()
{
	//Create the hitbox
}

void BossStateThirdRight::Enter()
{
	boss->enemyController->anim->SendTriggerToStateMachine(trigger.c_str());
	duration = boss->enemyController->anim->GetDurationFromClip();
}

void BossStateThirdRight::Exit()
{
}