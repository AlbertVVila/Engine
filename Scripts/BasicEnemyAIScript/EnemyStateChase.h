#ifndef __ENEMYSTATECHASE_H_
#define __ENEMYSTATECHASE_H_

#include "EnemyState.h"

class EnemyStateChase :
	public EnemyState
{
public:
	EnemyStateChase(BasicEnemyAIScript* AIScript);
	~EnemyStateChase();

	void HandleIA() override;
	void Update() override;
};

#endif __ENEMYSTATECHASE_H_