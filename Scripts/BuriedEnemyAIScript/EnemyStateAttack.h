#ifndef __ENEMYSTATEATTACK_H_
#define __ENEMYSTATEATTACK_H_

#include "EnemyState.h"

class EnemyStateAttack :
	public EnemyState
{
public:
	EnemyStateAttack(BuriedEnemyAIScript* AIScript);
	~EnemyStateAttack();
};

#endif // __ENEMYSTATEATTACK_H_
