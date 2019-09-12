#include "BossBehaviourScript.h"

#include "EnemyControllerScript/EnemyControllerScript.h"

#include "GameObject.h"

#include "BossState.h"
#include "BossStateIdle.h"
#include "BossStateNotActive.h"
#include "BossStateActivated.h"
#include "BossStateDeath.h"
#include "BossStateCast.h"
#include "BossStatePreCast.h"
#include "BossStateInterPhase.h"


BossBehaviourScript_API Script* CreateScript()
{
	BossBehaviourScript* instance = new BossBehaviourScript;
	return instance;
}

void BossBehaviourScript::Awake()
{
	// Look for Enemy Controller Script of the enemy
	enemyController = gameobject->GetComponent<EnemyControllerScript>();
	if (enemyController == nullptr)
	{
		LOG("The GameObject %s has no Enemy Controller Script component attached \n", gameobject->name.c_str());
	}

}

void BossBehaviourScript::Start()
{
	bossStates.reserve(7);
	bossStates.push_back(notActive = new BossStateNotActive(this));
	bossStates.push_back(activated = new BossStateActivated(this));
	bossStates.push_back(idle = new BossStateIdle(this));
	bossStates.push_back(cast = new BossStateCast(this));
	bossStates.push_back(precast = new BossStatePreCast(this));
	bossStates.push_back(interPhase = new BossStateInterPhase(this));
	bossStates.push_back(death = new BossStateDeath(this));

	currentState = notActive;
	bossPhase = BossPhase::None;
}

void BossBehaviourScript::Update()
{
	BossState* previous = currentState;

	CheckHealth();

	currentState->HandleIA();

	CheckStates(previous);

	currentState->Update();
}

void BossBehaviourScript::Expose(ImGuiContext * context)
{
}

void BossBehaviourScript::Serialize(JSON_value * json) const
{
}

void BossBehaviourScript::DeSerialize(JSON_value * json)
{
}

void BossBehaviourScript::CheckStates(BossState * previous)
{
	if (previous != currentState)
	{
		previous->Exit();
		currentState->Enter();
	}
}

void BossBehaviourScript::CheckHealth()
{
	float actualHealth = enemyController->GetHealth();
	switch (bossPhase)
	{
		case BossPhase::None:
			break;
		case BossPhase::First:
			if (actualHealth < firstHealthThreshold)
			{
				bossPhase = BossPhase::Second;
			}
			break;
		case BossPhase::Second:
			if (actualHealth < secondHealthThreshold)
			{
				bossPhase = BossPhase::Third;
			}
			break;
		case BossPhase::Third:
			break;
	}


}