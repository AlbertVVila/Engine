#ifndef _BOSSSTATEACTIVATED_H_
#define _BOSSSTATEACTIVATED_H_

#include "BossState.h"

class BossStateActivated :
	public BossState
{
public:
	BossStateActivated(BossBehaviourScript* AIBoss);
	~BossStateActivated();

private:
	void HandleIA() override;
	void Update() override;
};

#endif // _BOSSSTATEACTIVATED_H_