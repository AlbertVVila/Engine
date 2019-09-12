#ifndef _BOSSSTATENOTACTIVE_H_
#define _BOSSSTATENOTACTIVE_H_

#include "BossState.h"

class BossStateNotActive :
	public BossState
{
public:
	BossStateNotActive(BossBehaviourScript* AIBoss);
	~BossStateNotActive();
};

#endif // _BOSSSTATENOTACTIVE_H_