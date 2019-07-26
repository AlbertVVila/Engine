#ifndef __ENEMYSTATEFLEE_H_
#define __ENEMYSTATEFLEE_H_

#include "EnemyState.h"
class EnemyStateFlee :
	public EnemyState
{
public:
	EnemyStateFlee(BasicEnemyAIScript* AIScript);
	~EnemyStateFlee();

	void Enter() override;
	void HandleIA() override;
	void Update() override;

private:
	void MoveAwayFromPlayer();
};

#endif // __ENEMYSTATEFLEE_H_