#ifndef  __BasicEnemyAIScript_h__
#define  __BasicEnemyAIScript_h__

#include "BaseScript.h"

#ifdef BasicEnemyAIScript_EXPORTS
#define BasicEnemyAIScript_API __declspec(dllexport)
#else
#define BasicEnemyAIScript_API __declspec(dllimport)
#endif

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

public:
	EnemyState enemyState = EnemyState::STANDUP;

	// Stand-Up variables
	float standupSpeed = 1.0f;
	float yTranslation = 20.0f;

private:
	float auxTranslation = 0.0f;
};

#endif __BasicEnemyAIScript_h__
