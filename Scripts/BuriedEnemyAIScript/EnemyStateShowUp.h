#ifndef __ENEMYSTATESHOWUP_H_
#define __ENEMYSTATESHOWUP_H_

#include "EnemyState.h"
class EnemyStateShowUp :
	public EnemyState
{
public:
	EnemyStateShowUp(BuriedEnemyAIScript* AIScript);
	~EnemyStateShowUp();
};

#endif // __ENEMYSTATESHOWUP_H_