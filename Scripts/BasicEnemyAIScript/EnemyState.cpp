#include "EnemyState.h"

#include "Application.h"
#include "ModuleTime.h"

#include "BasicEnemyAIScript.h"

EnemyState::EnemyState()
{
}


EnemyState::~EnemyState()
{
}

void EnemyState::UpdateTimer()
{
	timer += enemy->Appl->time->gameDeltaTime;
}