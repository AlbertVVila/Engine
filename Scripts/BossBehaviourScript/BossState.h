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

	void UpdateTimer();
	void ResetTimer() { timer = 0.f; };

	float timer = 0.0f;

protected:
	BossBehaviourScript* boss = nullptr;
};

#endif // __BOSSSTATE_H_