#ifndef __BOSSSTATESECONDINTERPHASE_H_
#define __BOSSSTATESECONDINTERPHASE_H_

#include "BossState.h"
class BossStateSecondInterPhase :
	public BossState
{
public:
	BossStateSecondInterPhase(BossBehaviourScript* AIboss);
	~BossStateSecondInterPhase();

private:
	void HandleIA() override;
	void Update() override;
	void Enter() override;
	void Exit() override;
};

#endif 