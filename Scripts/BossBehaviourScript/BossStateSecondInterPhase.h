#ifndef __BOSSSTATESECONDINTERPHASE_H_
#define __BOSSSTATESECONDINTERPHASE_H_

#include "BossState.h"

enum class InterphaseState
{
	None,
	Kneel,
	Cry,
	FadeOff,
	Teleport,
	FloorVanish,
	FadeIn,
	Finished
};

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

	InterphaseState state = InterphaseState::None;

	bool finished = false;
};

#endif 