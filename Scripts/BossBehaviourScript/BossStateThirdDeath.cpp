#include "BossStateThirdDeath.h"

#include "BossBehaviourScript.h"

#include "EnemyControllerScript/EnemyControllerScript.h"
#include "ComponentAnimation.h"

BossStateThirdDeath::BossStateThirdDeath(BossBehaviourScript* AIBoss)
{
	boss = AIBoss;
	trigger = "ThirdDeath";
}


BossStateThirdDeath::~BossStateThirdDeath()
{
}

void BossStateThirdDeath::HandleIA()
{

}

void BossStateThirdDeath::Update()
{
	
}

void BossStateThirdDeath::Enter()
{
	boss->enemyController->anim->SendTriggerToStateMachine(trigger.c_str());
}

void BossStateThirdDeath::Exit()
{
}
