#ifndef __ENEMYSTATEATTACKSPIN_H_
#define __ENEMYSTATEATTACKSPIN_H_

#include  "BasicEnemyAIScript/EnemyStateAttack.h"
#include "Algorithm/Random/LCG.h"

class ComponentRenderer;

class EnemyStateAttackSpin :
	public EnemyStateAttack
{
public:
	EnemyStateAttackSpin(BasicEnemyAIScript* AIScript);
	~EnemyStateAttackSpin();

	void HandleIA() override;
	void Update() override;

protected:

	void Attack() override;

	ComponentRenderer* enemyRenderer = nullptr;

	LCG lcg;
	float spinDuration = 2.0f;
	float spinCooldown = 4.0f;

	float spinTimer = 0.0f;
	bool spinning = false;
	bool isOnCooldown = false;
};

#endif __ENEMYSTATEATTACKSPIN_H_