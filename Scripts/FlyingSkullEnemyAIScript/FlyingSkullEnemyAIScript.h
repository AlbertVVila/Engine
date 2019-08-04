#ifndef  __FlyingSkullEnemyAIScript_h__
#define  __FlyingSkullEnemyAIScript_h__

#ifdef FlyingSkullEnemyAIScript_EXPORTS
#define FlyingSkullEnemyAIScript_API __declspec(dllexport)
#else
#define FlyingSkullEnemyAIScript_API __declspec(dllimport)
#endif

#include "BaseScript.h"
#include "math/float3.h"
#include <vector>

class GameObject;
class EnemyControllerScript;
class ProjectileScript;
class JSON_value;
class EnemyState;
class EnemyStatePatrol;
class EnemyStateChase;
class EnemyStateReturn;
class EnemyStateAttack;
class EnemyStateCooldown;
class EnemyStateDeath;
class EnemyStateFlee;


class FlyingSkullEnemyAIScript_API FlyingSkullEnemyAIScript : public Script
{
	void Awake() override;
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context) override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	inline virtual FlyingSkullEnemyAIScript* Clone() const
	{
		return new FlyingSkullEnemyAIScript(*this);
	}

public:

	float cooldownDuration = 1.0f;
	float activationDistance = 1000.0f;
	float disengageDistance = 1500.0f;
	float maxAttackRange = 700.0f;
	float minAttackRange = 500.0f;
	float runSpeed = 150.0f;
	float offsetHeight = 120.0f;

	float movementConstant = 20.0f;
	float idleMovementSpeed = 5.0f;
	float acceleration = 0.0f;
	float angle = 0.0f;

	float projectileDelay = 0.5f;

	GameObject* projectileGO = nullptr;
	ProjectileScript* projectileScript = nullptr;

public:
	//hierarchy

	GameObject* boneCenter = nullptr;
	GameObject* spinningBones = nullptr;
	GameObject* particles = nullptr;

	void Movement();

public:

	EnemyStateAttack* attack = nullptr;
	EnemyStateChase* chase = nullptr;
	EnemyStateFlee* flee = nullptr;
	EnemyStateCooldown* cooldown = nullptr;
	EnemyStateReturn* returnToStart = nullptr;
	EnemyStateDeath* death = nullptr;
	EnemyStatePatrol* patrol = nullptr;

	EnemyState* currentState = nullptr;

	EnemyControllerScript* enemyController = nullptr;

	math::float3 startPosition = math::float3::zero;
private:
	std::vector<EnemyState*> enemyStates;

	void CheckStates(EnemyState* previous);
};

#endif __FlyingSkullEnemyAIScript_h__
