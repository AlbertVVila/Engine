#ifndef  __EnemyControllerScript_h__
#define  __EnemyControllerScript_h__

#include "BaseScript.h"

#include "Geometry/AABB.h"

#ifdef EnemyControllerScript_EXPORTS
#define EnemyControllerScript_API __declspec(dllexport)
#else
#define EnemyControllerScript_API __declspec(dllimport)
#endif

class ComponentRenderer;
class DamageController;

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

public:
	GameObject* player = nullptr;
	GameObject* enemyLife = nullptr;
	std::string playerName = "Player";
	std::string playerBboxName = "PlayerMesh";
	ComponentRenderer* myRender;
	std::string myBboxName = "EnemyMesh";

	DamageController* damageController = nullptr;
	
	// BBoxes
	math::AABB* myBbox = nullptr;
	math::AABB* playerBbox = nullptr;

private:
	int actualHealth = 20;
	int maxHealth = 20;
};

#endif __EnemyControllerScript_h__
