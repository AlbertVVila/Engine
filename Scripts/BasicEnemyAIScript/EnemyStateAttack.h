#ifndef __ENEMYSTATEATTACK_H_
#define __ENEMYSTATEATTACK_H_

#include "EnemyState.h"

class EnemyStateAttack :
	public EnemyState
{
public:
	EnemyStateAttack(BasicEnemyAIScript* AIScript);
	~EnemyStateAttack();

	void HandleIA() override;
	void Update() override;

private:

	bool attacked = false;
	void Attack();
};

#endif __ENEMYSTATEATTACK_H_