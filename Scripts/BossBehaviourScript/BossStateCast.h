#ifndef _BOSSSTATECAST_H_
#define _BOSSSTATECAST_H_

#include "BossState.h"

class BossStateCast :
	public BossState
{
public:
	BossStateCast(BossBehaviourScript* AIBoss);
	~BossStateCast();
};

#endif // _BOSSSTATECAST_H_