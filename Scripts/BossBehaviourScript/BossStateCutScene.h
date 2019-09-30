#ifndef _BOSSSTATECUTSCENE_H_
#define _BOSSSTATECUTSCENE_H_

#include "BossState.h"

class BossStateCutScene :
	public BossState
{
public:
	BossStateCutScene(BossBehaviourScript* AIBoss);
	~BossStateCutScene();

private:

	void HandleIA() override;
	void Update() override;

	void Enter() override;
	void Exit() override;
};

#endif