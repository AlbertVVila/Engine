#ifndef __ENEMYSTATECHASE_H_
#define __ENEMYSTATECHASE_H_

#include "EnemyState.h"
class EnemyStateChase :
	public EnemyState
{
public:
	EnemyStateChase(BuriedEnemyAIScript* AIScript);
	~EnemyStateChase();
};

#endif // __ENEMYSTATECHASE_H_

