#ifndef __BOSSSTATETHIRDLEFT_H_
#define __BOSSSTATETHIRDLEFT_H_

#include "BossState.h"

class BossStateThirdLeft :
	public BossState
{
public:
	BossStateThirdLeft(BossBehaviourScript* AIBoss);
	~BossStateThirdLeft();

	void HandleIA() override;
	void Update() override;

	void Enter() override;
	void Exit() override;
};

#endif // __BOSSSTATETHIRDLEFT_H_
