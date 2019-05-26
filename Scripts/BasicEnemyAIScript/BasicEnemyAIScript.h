#ifndef  __BasicEnemyAIScript_h__
#define  __BasicEnemyAIScript_h__

#ifdef BasicEnemyAIScript_EXPORTS
#define BasicEnemyAIScript_API __declspec(dllexport)
#else
#define BasicEnemyAIScript_API __declspec(dllimport)
#endif

#include "BaseScript.h"
#include "Math/float3.h"
#include <vector>

class ComponentAnimation;
class ComponentBoxTrigger;
class PlayerMovement;
class EnemyControllerScript;
class JSON_value;
class EnemyState;
class EnemyStatePatrol;
class EnemyStateChase;
class EnemyStateReturnToStart;
class EnemyStateAttack;
class EnemyStateCooldown;
class EnemyStateDeath;


class BasicEnemyAIScript_API BasicEnemyAIScript : public Script
{
public:
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context) override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	inline math::float3 GetPosition() const;					// Get position of the enemy (GO with this script attached)
	inline math::float3 GetPlayerPosition() const;
	inline float GetDistanceTo(math::float3& position) const;	// Get distance of the enemy to position given as argument
	inline float GetDistanceTo2D(math::float3& position) const;	// Get distance of the enemy to position given as argument only taking XZ plane as reference

	inline bool IsCollidingWithPlayer() const;

	void MoveTowards(float speed) const;
	void LookAt2D(math::float3& position);

private:
	void CheckStates(EnemyState* previous, EnemyState* current);

public:
	Application* Appl = nullptr;
	EnemyState* currentState = nullptr;

	EnemyStatePatrol* patrol = nullptr;
	EnemyStateChase* chase = nullptr;
	EnemyStateReturnToStart* returnToStart = nullptr;
	EnemyStateAttack* attack = nullptr;
	EnemyStateCooldown* cooldown = nullptr;
	EnemyStateDeath* death = nullptr;

	// Patrol variables
	float activationDistance = 100.0f;	// Distance to player needed to start chasing the player (only X,Z axis is taken into account)

	// Chase variables
	float chaseSpeed = 2.0f;			// Tranlation speed when chasing player

	// Return variables
	math::float3 startPosition;
	float returnDistance = 150.f;		// Distance to player to stop chasing player and return to start position
	float returnSpeed = 1.0f;			// Tranlation speed towards start position

	// Attack variables
	float attackDuration = 1.0f;
	float attackDamage = 20.0f;

	// Cooldown variables
	float cooldownTime = 1.0f;			// Seconds to wait between attacks

	ComponentBoxTrigger* boxTrigger = nullptr;

private:

	ComponentAnimation* anim = nullptr;

	EnemyControllerScript* enemyController;
	PlayerMovement* playerScript;

	std::vector<EnemyState*> enemyStates;
};

#endif __BasicEnemyAIScript_h__
