#ifndef __ENEMYSTATEATTACK_H_
#define __ENEMYSTATEATTACK_H_

#include "EnemyState.h"

class EnemyStateAttack :
	public EnemyState
{
public:
	EnemyStateAttack(RangeEnemyAIScript* AIScript);
	~EnemyStateAttack();

	void Enter() override;
	void HandleIA() override;
	void Update() override;

private:
	bool projShot1 = false;
	bool projShot2 = false;
	bool projShot3 = false;

};

#endif __ENEMYSTATEATTACK_H_