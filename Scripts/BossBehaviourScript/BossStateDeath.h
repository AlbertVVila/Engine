#ifndef __BOSSSTATEDEATH_H_
#define __BOSSSTATEDEATH_H_

#include "BossState.h"

class BossStateDeath :
	public BossState
{
public:
	BossStateDeath(BossBehaviourScript* AIBoss);
	~BossStateDeath();
};

#endif // __BOSSSTATEDEATH_H_