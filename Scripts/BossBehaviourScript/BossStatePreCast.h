#ifndef __BOSSSTATEPRECAST_H_
#define __BOSSSTATEPRECAST_H_

#include "BossState.h"

class BossStatePreCast :
	public BossState
{
public:
	BossStatePreCast(BossBehaviourScript* AIBoss);
	~BossStatePreCast();
};

#endif // __BOSSSTATEPRECAST_H_