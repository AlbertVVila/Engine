#ifndef __ENEMYSTATEHIDE_H_
#define __ENEMYSTATEHIDE_H_

#include "EnemyState.h"
class EnemyStateHide :
	public EnemyState
{
public:
	EnemyStateHide(BuriedEnemyAIScript* AIScript);
	~EnemyStateHide();
};

#endif // __ENEMYSTATEHIDE_H_