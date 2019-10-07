#ifndef __BOSSSTATETHIRDRIGHT_H_
#define __BOSSSTATETHIRDRIGHT_H_

#include "BossState.h"

class BossStateThirdRight :
	public BossState
{
public:
	BossStateThirdRight(BossBehaviourScript* AIBoss);
	~BossStateThirdRight();

	void HandleIA() override;
	void Update() override;

	void Enter() override;
	void Exit() override;
};

#endif // __BOSSSTATETHIRDRIGHT_H_
