#ifndef  __CircularAttackSkill_h__
#define  __CircularAttackSkill_h__

#define MESH1_ROT_SPEED 0.5f
#define MESH2_ROT_SPEED 1.0f
#define MESH3_ROT_SPEED 1.5f

#include "MeleeSkill.h"

#include <vector>
#include "Math/float3.h"

class GameObject;

class CircularAttackSkill :
	public MeleeSkill
{
public:
	CircularAttackSkill(PlayerMovement* PM, const char* trigger, ComponentBoxTrigger* attackBox);
	~CircularAttackSkill();

	void Start() override;
	void Update() override;

	GameObject* mesh1 = nullptr;
	GameObject* mesh2 = nullptr;
	GameObject* mesh3 = nullptr;
	GameObject* particles = nullptr;


protected:
	void UseSkill() override;
	void Reset() override;

	void CheckInput() override;
	void MoveSpinning();

private:
	float fullSpinTime = 0.0f;	// Time it takes to the player to draw a full circle spinning (auto-calculated)
	float spinTimer = 0.0f;		// Timer to check time performing a skill

	unsigned numSpins = 4u;		// Number of complete spins the player will make before stoping

	bool atatckStarted = false;

	// Move variables
	float duration = 1.5f;
	std::vector<float3>path;
	unsigned pathIndex = 0;
	float moveTimer = 0.0f;


};

#endif __CircularAttackSkill_h__