#ifndef __BOSSSTATETHIRDIDLE_H_
#define __BOSSSTATETHIRDIDLE_H_

#include "BossState.h"
class BossStateThirdIdle :
	public BossState
{
public:
	BossStateThirdIdle(BossBehaviourScript* AIBoss);
	~BossStateThirdIdle();

	void HandleIA() override;
	void Update() override;

	void Enter() override;
	void Exit() override;
};

#endif // __BOSSSTATETHIRDIDLE_H_
