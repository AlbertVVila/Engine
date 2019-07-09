#ifndef __ENEMYSTATESHOWUP_H_
#define __ENEMYSTATESHOWUP_H_

#include "EnemyState.h"

class EnemyStateShowUp :
	public EnemyState
{
public:
	EnemyStateShowUp(HiddenEnemyAIScript* AIScript);
	~EnemyStateShowUp();

	void HandleIA() override;
	void Update() override;
};

#endif __ENEMYSTATESHOWUP_H_