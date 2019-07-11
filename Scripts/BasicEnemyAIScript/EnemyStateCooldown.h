#ifndef __ENEMYSTATECOOLDOWN_H_
#define __ENEMYSTATECOOLDOWN_H_

#include "EnemyState.h"

class EnemyStateCooldown :
	public EnemyState
{
public:
	EnemyStateCooldown(BasicEnemyAIScript* AIScript);
	~EnemyStateCooldown();

	void HandleIA() override;
	void Update() override;

private:
	float waitedTime = 1.f;
};

#endif __ENEMYSTATECOOLDOWN_H_