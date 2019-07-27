#include "DividingEnemyAI.h"

#include "BasicEnemyAIScript/EnemyState.h"
#include "BasicEnemyAIScript/EnemyStatePatrol.h"
#include "BasicEnemyAIScript/EnemyStateChase.h"
#include "BasicEnemyAIScript/EnemyStateReturnToStart.h"
#include "BasicEnemyAIScript/EnemyStateAttack.h"
#include "BasicEnemyAIScript/EnemyStateCooldown.h"
#include "BasicEnemyAIScript/EnemyStateFlee.h"
#include "BasicEnemyAIScript/EnemyStateDeath.h"
#include "EnemyStateMultiply.h"

DividingEnemyAI_API Script* CreateScript()
{
	DividingEnemyAI* instance = new DividingEnemyAI;
	return instance;
}

void DividingEnemyAI::Start()
{
	BasicEnemyAIScript::Start();
	enemyStates.push_back(multiply = new EnemyStateMultiply(this));
}