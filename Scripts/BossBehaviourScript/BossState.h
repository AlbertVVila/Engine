#ifndef _BOSSSTATE_H_
#define _BOSSSTATE_H_

class BossBehaviourScript;

class BossState
{
public:
	BossState();
	virtual ~BossState();

private:
	BossBehaviourScript* boss = nullptr;
};

#endif // __BOSSSTATE_H_