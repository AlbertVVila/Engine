#ifndef  __BossBehaviourScript_h__
#define  __BossBehaviourScript_h__

#include "BaseScript.h"
#include <vector>
#include "Math/float3.h"

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
class BossStateDeath;
class BossStateNotActive;
class BossStateActivated;
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

	BossStateActivated* activated = nullptr;
	BossStateNotActive* notActive = nullptr;
	BossStateIdle* idle = nullptr;
	BossStatePreCast* precast = nullptr;
	BossStateInterPhase* interPhase = nullptr;
	BossStateDeath* death = nullptr;
	BossStateCast* cast = nullptr;

public:
	float firstHealthThreshold = 0.75f;
	float secondHealthThreshold = 0.35f;

	float distanceToPlayer = 0.0f;
	float activationDistance = 800.0f;
	float activationTime = 5.0f;

	math::float3 startingPoint = math::float3::zero;
	math::float3 highPointFirstCS = math::float3::zero;
	math::float3 pointStartFight = math::float3::zero;

private:
	std::vector<BossState*> bossStates;

	void CheckStates(BossState* previous);
	void CheckHealth();
	void FloatInSpace();
	float ReturnDistanceToPlayer();


	bool isFloating = true;
	float angleConstant = 1.0f;
	float floatConstant = 1.0f;
	float yOffset = 0.0f;
	float angle = 0.0f;
};

#endif __BossBehaviourScript_h__
