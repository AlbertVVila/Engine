#pragma once
#include "EnemyState.h"
class EnemyStateDeath :
	public EnemyState
{
public:
	BasicEnemyAIScript_API EnemyStateDeath(BasicEnemyAIScript* AIScript);
	BasicEnemyAIScript_API ~EnemyStateDeath();


	void Enter() override;
	void Update() override;
};

