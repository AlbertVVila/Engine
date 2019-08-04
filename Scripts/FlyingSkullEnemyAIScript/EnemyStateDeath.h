#ifndef __ENEMYSTATEDEATH_H_
#define __ENEMYSTATEDEATH_H_

#include "EnemyState.h"
class EnemyStateDeath :
	public EnemyState
{
public:
	EnemyStateDeath(FlyingSkullEnemyAIScript* AIScript);
	~EnemyStateDeath();

	void HandleIA() override;
	void Update() override;
};

#endif // __ENEMYSTATEDEATH_H_