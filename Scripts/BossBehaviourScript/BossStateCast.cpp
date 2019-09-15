#include "Application.h"
#include "ModuleTime.h"
#include "ModuleScene.h"

#include "BossStateCast.h"

#include "BossBehaviourScript.h"
#include "EnemyControllerScript/EnemyControllerScript.h"

BossStateCast::BossStateCast(BossBehaviourScript* AIBoss)
{
	boss = AIBoss;
}


BossStateCast::~BossStateCast()
{
}

void BossStateCast::HandleIA()
{
	if (timer > duration)
	{
		boss->currentState = (BossState*)boss->idle;
	}
}

void BossStateCast::Update()
{
	boss->enemyController->LookAt2D(boss->playerPosition);
}

void BossStateCast::Enter()
{
	duration = 1.0f;
	BossSkill nextSkill = SelectSkillToUse();

	switch (nextSkill)
	{
	case BossSkill::Aoe:
		boss->circlesSpawning = true;
		break;
	case BossSkill::Teleport:
		boss->bossTeleporting = true;
		break;
	case BossSkill::Summon:
		boss->bossSummoning = true;
		break;
	}
	//I can input here different durations for each skill
}

BossSkill BossStateCast::SelectSkillToUse()
{
	int randomNumb = rand() % 2;

	BossSkill skillToCast;

	switch (boss->lastUsedSkill)
	{
	case BossSkill::None:
		skillToCast = BossSkill::Teleport;
		break;
	case BossSkill::Aoe:
		if (randomNumb == 0)
		{
			skillToCast = BossSkill::Summon;
		}
		else if (randomNumb == 1)
		{
			skillToCast = BossSkill::Teleport;
		}
		break;
	case BossSkill::Summon:
		if (randomNumb == 0)
		{
			skillToCast = BossSkill::Aoe;
		}
		else if (randomNumb == 1)
		{
			skillToCast = BossSkill::Teleport;
		}
		break;
	case BossSkill::Teleport:
		if (randomNumb == 0)
		{
			skillToCast = BossSkill::Summon;
		}
		else if (randomNumb == 1)
		{
			skillToCast = BossSkill::Aoe;
		}
		break;
	}

	boss->secondLastUsedSkill = boss->lastUsedSkill;
	boss->lastUsedSkill = skillToCast;

	return skillToCast;
	
}
