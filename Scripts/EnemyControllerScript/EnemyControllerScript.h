#ifndef  __EnemyControllerScript_h__
#define  __EnemyControllerScript_h__

#include "BaseScript.h"

#include "Geometry/AABB.h"

#ifdef EnemyControllerScript_EXPORTS
#define EnemyControllerScript_API __declspec(dllexport)
#else
#define EnemyControllerScript_API __declspec(dllimport)
#endif

class ComponentAnimation;
class DamageController;
class EnemyLifeBarController;

class EnemyControllerScript_API EnemyControllerScript : public Script
{
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context) override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

public:
	void TakeDamage(unsigned damage);
	int GetHealth() const { return actualHealth; }

	inline math::float3 GetPosition() const;					// Get position of the enemy (GO with this script attached)
	inline math::float3 GetPlayerPosition() const;
	inline float GetDistanceTo(math::float3& position) const;	// Get distance of the enemy to position given as argument
	inline float GetDistanceTo2D(math::float3& position) const;	// Get distance of the enemy to position given as argument only taking XZ plane as reference
	inline float GetDistanceToPlayer2D() const;

	inline bool IsCollidingWithPlayer() const;

	void Move(float speed, math::float3& direction) const;
	void MoveTowards(float speed) const;
	void LookAt2D(math::float3& position);

public:
	GameObject* player = nullptr;
	std::string playerName = "Player";
	std::string playerBboxName = "PlayerMesh";
	std::string myBboxName = "EnemyMesh";
	ComponentAnimation* anim = nullptr;

	DamageController* damageController = nullptr;
	EnemyLifeBarController* enemyLifeBar = nullptr;
	
	// BBoxes
	math::AABB* myBbox = nullptr;
	math::AABB* playerBbox = nullptr;

private:
	int actualHealth = 20;
	int maxHealth = 20;
};

#endif __EnemyControllerScript_h__
