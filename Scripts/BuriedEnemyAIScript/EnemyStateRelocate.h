#ifndef __ENEMYSTATERELOCATE_H_
#define __ENEMYSTATERELOCATE_H_

#include "EnemyState.h"
class EnemyStateRelocate :
	public EnemyState
{
public:
	EnemyStateRelocate(BuriedEnemyAIScript* AIScript);
	~EnemyStateRelocate();
};

#endif // __ENEMYSTATERELOCATE_H_