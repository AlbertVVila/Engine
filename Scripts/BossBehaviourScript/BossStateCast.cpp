#include "BossStateCast.h"

#include "BossBehaviourScript.h"

BossStateCast::BossStateCast(BossBehaviourScript* AIBoss)
{
	boss = AIBoss;
}


BossStateCast::~BossStateCast()
{
}

void BossStateCast::HandleIA()
{

}

void BossStateCast::Update()
{
}

void BossStateCast::Enter()
{
	boss->circlesSpawning = true;
}
