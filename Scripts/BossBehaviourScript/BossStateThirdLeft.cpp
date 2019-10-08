#include "BossStateThirdLeft.h"

#include "BossBehaviourScript.h"

#include "EnemyControllerScript/EnemyControllerScript.h"
#include "ComponentAnimation.h"


BossStateThirdLeft::BossStateThirdLeft(BossBehaviourScript* AIBoss)
{
	boss = AIBoss;
	trigger = "ThirdLeft";
}


BossStateThirdLeft::~BossStateThirdLeft()
{
}

void BossStateThirdLeft::HandleIA()
{
	if (duration < timer)
	{
		boss->currentState = (BossState*)boss->thirdIdle;
	}
}

void BossStateThirdLeft::Update()
{
	//Need to create the hitbox
}

void BossStateThirdLeft::Enter()
{
	boss->enemyController->anim->SendTriggerToStateMachine(trigger.c_str());
	duration = boss->enemyController->anim->GetDurationFromClip();
}

void BossStateThirdLeft::Exit()
{
}
