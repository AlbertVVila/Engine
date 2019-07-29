#ifndef __ENEMYSTATE_H_
#define __ENEMYSTATE_H_

class BuriedEnemyAIScript;

class EnemyState
{
public:
	EnemyState();
	virtual ~EnemyState();

	BuriedEnemyAIScript* enemy = nullptr;
};

#endif // __ENEMYSTATE_H_