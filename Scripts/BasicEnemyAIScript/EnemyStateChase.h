#ifndef __ENEMYSTATECHASE_H_
#define __ENEMYSTATECHASE_H_

#include "EnemyState.h"

class EnemyStateChase :
	public EnemyState
{
public:
	BasicEnemyAIScript_API EnemyStateChase(BasicEnemyAIScript* AIScript);
	BasicEnemyAIScript_API ~EnemyStateChase();

	void HandleIA() override;
	void Update() override;
};

#endif __ENEMYSTATECHASE_H_