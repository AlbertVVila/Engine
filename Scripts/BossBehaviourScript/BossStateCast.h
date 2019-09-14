#ifndef _BOSSSTATECAST_H_
#define _BOSSSTATECAST_H_

#include "BossState.h"

class BossStateCast :
	public BossState
{
public:
	BossStateCast(BossBehaviourScript* AIBoss);
	~BossStateCast();

	void HandleIA() override;
	void Update() override;

	void Enter() override;
};

#endif // _BOSSSTATECAST_H_