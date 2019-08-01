#ifndef __ENEMYSTATEATTACK_H_
#define __ENEMYSTATEATTACK_H_

#include "EnemyState.h"
class ComponentTrail;

class EnemyStateAttack :
	public EnemyState
{
public:
	BasicEnemyAIScript_API EnemyStateAttack(BasicEnemyAIScript* AIScript);
	BasicEnemyAIScript_API ~EnemyStateAttack();

	void HandleIA() override;
	void Update() override;

	ComponentTrail* trailPunch = nullptr;

private:
	void PunchFX(bool active);

	bool attacked = false;
	void Attack();
};

#endif __ENEMYSTATEATTACK_H_