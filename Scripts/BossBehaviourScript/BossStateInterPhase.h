#ifndef __BOSSSTATEINTERPHASE_H_
#define __BOSSSTATEINTERPHASE_H_

#include "BossState.h"
class BossStateInterPhase :
	public BossState
{
public:
	BossStateInterPhase(BossBehaviourScript* AIBoss);
	~BossStateInterPhase();

private:

	void HandleIA() override;
	void Update() override;
	void Enter() override;
	void Exit() override;
};

#endif // __BOSSSTATEINTERPHASE_H_