#ifndef __ENEMYSTATEDEATH_H_
#define __ENEMYSTATEDEATH_H_

#include "EnemyState.h"

class EnemyStateDeath :
	public EnemyState
{
public:
	EnemyStateDeath(BuriedEnemyAIScript* AIScript);
	~EnemyStateDeath();
};

#endif // __ENEMYSTATEDEATH_H_