#ifndef  __BuriedEnemyAIScript_h_
#define  __BuriedEnemyAIScript_h_

#ifdef BuriedEnemyAIScript_EXPORTS
#define BuriedEnemyAIScript_API __declspec(dllexport)
#else
#define BuriedEnemyAIScript_API __declspec(dllimport)
#endif

#include "BaseScript.h"
#include "math/float3.h"
#include <vector>

class EnemyControllerScript;
class JSON_value;
class EnemyState;
class EnemyStatePatrol;
class EnemyStateHide;
class EnemyStateHidden;
class EnemyStateRelocate;
class EnemyStateChase;
class EnemyStateShowUp;
class EnemyStateReturnToStart;
class EnemyStateAttack;
class EnemyStateCooldown;
class EnemyStateDeath;
class EnemyStateFlee;

class BuriedEnemyAIScript_API BuriedEnemyAIScript : public Script
{
public:
	void Awake() override;
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context) override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	inline virtual BuriedEnemyAIScript* Clone() const
	{
		return new BuriedEnemyAIScript(*this);
	}

	void CheckStates(EnemyState* previous);


public:

	EnemyStateAttack* attack = nullptr;
	EnemyStateChase* chase = nullptr;
	EnemyStateCooldown* cooldown = nullptr;
	EnemyStateHide* hide = nullptr;
	EnemyStateHidden* hidden = nullptr;
	EnemyStateRelocate* relocate = nullptr;
	EnemyStateShowUp* showUp = nullptr;
	EnemyStateReturnToStart* returnToStart = nullptr;
	EnemyStateDeath* death = nullptr;

	EnemyState* currentState = nullptr;

	float cooldownDuration = 1.0f;
	float activationDistance = 1000.0f;
	float disengageDistance = 1500.0f;
	float maxAttackRange = 700.0f;
	float minAttackRange = 500.0f;
	float teleportCooldown = 5.0f;
	float runSpeed = 150.0f;

	bool teleportAvailable = true;

private:

	std::vector<EnemyState*> enemyStates;

public:

	EnemyControllerScript* enemyController = nullptr;
};

extern "C" BuriedEnemyAIScript_API Script* CreateScript();

#endif __BuriedEnemyAIScript_h_
