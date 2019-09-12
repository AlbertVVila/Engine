#ifndef _BOSSSTATE_H_
#define _BOSSSTATE_H_

class BossBehaviourScript;

class BossState
{
public:
	BossState();
	virtual ~BossState();

	virtual void HandleIA() {};
	virtual void Update() {};
	virtual void Enter() {};
	virtual void Exit() {};

protected:
	BossBehaviourScript* boss = nullptr;
};

#endif // __BOSSSTATE_H_