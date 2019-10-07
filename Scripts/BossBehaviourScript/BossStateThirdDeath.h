#ifndef __BOSSSTATETHIRDEATH_H_
#define __BOSSSTATETHIRDEATH_H_

#include "BossState.h"
class BossStateThirdDeath :
	public BossState
{
public:
	BossStateThirdDeath(BossBehaviourScript* AIBoss);
	~BossStateThirdDeath();
};

#endif