#ifndef __ENEMYSTATERETURN_H_
#define __ENEMYSTATERETURN_H_

#include "EnemyState.h"
class EnemyStateReturnToStart :
	public EnemyState
{
public:
	EnemyStateReturnToStart(BuriedEnemyAIScript* AIScript);
	~EnemyStateReturnToStart();
};

#endif // __ENEMYSTATERETURN_H_