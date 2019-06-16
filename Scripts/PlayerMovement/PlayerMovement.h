#ifndef  __PlayerMovement_h__
#define  __PlayerMovement_h__

class GameObject;
class JSON_value;
struct ImGuiContext;

#include "BaseScript.h"
#include "Application.h"
#include "Math/float3.h"
#include <vector>

#define MINIMUM_PATH_DISTANCE 400.0f

#ifdef PlayerMovement_EXPORTS
#define PlayerMovement_API __declspec(dllexport)

#else
#define PlayerMovement_API __declspec(dllimport)
#endif

class ComponentAnimation;
class ComponentTransform;
class ComponentBoxTrigger;
class ComponentImage;
class JSON_value;
struct ImGuiContext;
class PlayerState;
class PlayerStateFirstAttack;
class PlayerStateSecondAttack;
class PlayerStateThirdAttack;
class PlayerStateIdle;
class PlayerStateDash;
class PlayerStateDeath;
class PlayerStateUppercut;
class PlayerStateWalk;
class DamageController;

struct PlayerMovement_API PlayerStats
{
public:
	void Serialize(JSON_value* json) const;
	void DeSerialize(JSON_value* json);
	void Expose(const char* sectionTitle);

	PlayerStats& operator+=(const PlayerStats& other) {
		this->health += other.health;
		this->mana += other.mana;
		this->strength += other.strength;
		this->dexterity += other.dexterity;
		this->manaRegen += other.manaRegen;
		this->hpRegen += other.hpRegen;
		return *this;
	}

	PlayerStats& operator-=(const PlayerStats& other)
	{
		this->health -= other.health;
		this->mana -= other.mana;
		this->strength -= other.strength;
		this->dexterity -= other.dexterity;
		this->manaRegen -= other.manaRegen;
		this->hpRegen -= other.hpRegen;
		return *this;
	}

public:
	float health = 0;
	float mana = 0;
	unsigned strength  = 0;
	unsigned dexterity = 0;

	float hpRegen = 0;
	float manaRegen = 0;
};

class PlayerMovement_API PlayerMovement : public Script
{
public:
	void Expose(ImGuiContext* context) override;

	void Start() override;
	void Update() override;
	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	void OnTriggerExit(GameObject* go) override;
	void Damage(float amount);

	void Equip(const PlayerStats& equipStats);
	void UnEquip(const PlayerStats& equipStats);

	//Abstract input
	bool IsAtacking() const;
	bool IsMoving() const;
	bool IsUsingFirstSkill() const;
	bool IsUsingSecondSkill() const;
	bool IsUsingThirdSkill() const;
	bool IsUsingFourthSkill() const;
	bool IsUsingFirstItem() const;
	bool IsUsingSecondItem() const;
	bool IsUsingThirdItem() const;
	bool IsUsingFourthItem() const;

private:
	void CheckStates(PlayerState* previous, PlayerState* current);
	void CreatePlayerStates();
	void ManaManagement();

public:
	bool isPlayerDead = false;
	float3 currentPosition = float3(0, 0, 0); //TODO ZERO

	PlayerStateFirstAttack* firstAttack = nullptr;
	PlayerStateSecondAttack* secondAttack = nullptr;
	PlayerStateThirdAttack* thirdAttack = nullptr;
	PlayerStateIdle* idle = nullptr;
	PlayerStateDash* dash = nullptr;
	PlayerStateDeath* death = nullptr;
	PlayerStateUppercut* uppercut = nullptr;
	PlayerStateWalk* walk = nullptr;

	float walkingSpeed = 100.0f;
	float dashSpeed = 10.0f;
	//const float fullHealth = 100.0f;
	float health = 100.0f;
	//const float fullMana = 100.0f;
	float mana = 100.0f;
	bool IsManaUsed = false;
	float attackDuration = 1.0f;
	float attackTimer = 0.0f;
	
	float outCombatTimer = 0.0f;
	float outCombatMaxTime = 3.0f;

	PlayerStats stats = { 100.0f, 100.0f, 10U, 10U, 5.0f, 5.0f };

	float OutOfMeshCorrectionXZ = 500.f;
	float OutOfMeshCorrectionY = 300.0f;
	ComponentAnimation* anim = nullptr;
	ComponentBoxTrigger* attackBoxTrigger = nullptr;
	ComponentBoxTrigger* hpHitBoxTrigger = nullptr;
	ComponentTransform* transform = nullptr;
	PlayerState* currentState = nullptr;

	math::float3 hpHitBoxSize = math::float3::zero;

	bool canInteract = true;

private:
	std::vector<PlayerState*> playerStates;	
	GameObject* dustParticles = nullptr;
	GameObject* dashFX = nullptr;
	GameObject* dashMesh = nullptr;

	DamageController* damageController = nullptr;
	ComponentImage* lifeUIComponent = nullptr;
	ComponentImage* manaUIComponent = nullptr;
};
#endif __PlayerMovement_h__
