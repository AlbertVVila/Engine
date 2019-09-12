#ifndef  __BossBehaviourScript_h__
#define  __BossBehaviourScript_h__

#include "BaseScript.h"

#ifdef BossBehaviourScript_EXPORTS
#define BossBehaviourScript_API __declspec(dllexport)
#else
#define BossBehaviourScript_API __declspec(dllimport)
#endif

enum class BossPhase
{
	None,
	First,
	Second,
	Third
};

class BossState;
class BossStateCast;
class BossStatePreCast;
class BossStateIdle;
class BossStateNotActive;
class BossStateInterPhase;
class EnemyControllerScript;

class BossBehaviourScript_API BossBehaviourScript : public Script
{
public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context) override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	inline virtual BossBehaviourScript* Clone() const
	{
		return new BossBehaviourScript(*this);
	}
public:

	BossPhase bossPhase = BossPhase::None;
	BossState* currentState = nullptr;

	EnemyControllerScript* enemyController = nullptr;
};

#endif __BossBehaviourScript_h__
