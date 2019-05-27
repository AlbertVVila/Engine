#pragma once
#include "EnemyState.h"
class EnemyStateDeath :
	public EnemyState
{
public:
	EnemyStateDeath(RangeEnemyAIScript* AIScript);
	~EnemyStateDeath();

	void Update() override;
};

