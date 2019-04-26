#ifndef  __BasicEnemyAIScript_h__
#define  __BasicEnemyAIScript_h__

#ifdef BasicEnemyAIScript_EXPORTS
#define BasicEnemyAIScript_API __declspec(dllexport)
#else
#define BasicEnemyAIScript_API __declspec(dllimport)
#endif

class GameObject;
class JSON_value;

#include "BaseScript.h"
#include "Geometry/AABB.h"

enum class EnemyState
{
	WAIT,
	STANDUP,
	CHASE,
	ATTACK,
	COOLDOWN,
	DEAD
};

class BasicEnemyAIScript_API BasicEnemyAIScript : public Script
{
public:
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context) override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

private:
	void Wait();
	void StandUp();
	void Chase();
	void Attack();

	void LookAtPlayer();

	GameObject* GetGameObjectRecursiveByName(GameObject* gameObject, const char* name);

public:
	EnemyState enemyState = EnemyState::WAIT;

private:
	GameObject* player = nullptr;
	std::string playerName = "Player";
	std::string playerBboxName = "PlayerMesh";
	std::string myBboxName = "EnemyMesh";

	// Wait variables
	float activationDistance = 100.0f;

	// Stand-Up variables
	float standupSpeed = 1.0f;
	float yTranslation = 20.0f;

	// Chase variables
	float chaseSpeed = 2.0f;

	float auxTranslation = 0.0f;

	math::AABB* myBbox = nullptr;
	math::AABB* playerBbox = nullptr;
};

#endif __BasicEnemyAIScript_h__
