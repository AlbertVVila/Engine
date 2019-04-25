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
	void StandUp();
	void Chase();

	GameObject* GetGameObjectRecursiveByName(GameObject* gameObject, const char* name);

public:
	EnemyState enemyState = EnemyState::STANDUP;

private:
	GameObject* player = nullptr;
	std::string playerName = "Player";
	std::string playerBboxName = "PlayerMesh";
	std::string myBboxName = "EnemyMesh";

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
