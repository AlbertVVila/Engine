#include "BossStatePreCast.h"

#include "BossBehaviourScript.h"
#include "EnemyControllerScript/EnemyControllerScript.h"
#include "ComponentAnimation.h"

BossStatePreCast::BossStatePreCast(BossBehaviourScript* AIBoss)
{
	boss = AIBoss;
	trigger = "PrecastNL";
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
	boss->enemyController->LookAt2D(boss->playerPosition);
}

void BossStatePreCast::Enter()
{
	boss->anim->SendTriggerToStateMachine(trigger.c_str());
	duration = boss->anim->GetDurationFromClip();
}
