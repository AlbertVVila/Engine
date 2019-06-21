#ifndef  __CircularAttackSkill_h__
#define  __CircularAttackSkill_h__

#include "MeleeSkill.h"

#include <vector>
#include "Math/float3.h"

class CircularAttackSkill :
	public MeleeSkill
{
public:
	CircularAttackSkill(PlayerMovement* PM, const char* trigger, ComponentBoxTrigger* attackBox);
	~CircularAttackSkill();

	void Start() override;
	void Update() override;

protected:
	void UseSkill() override;
	void Reset() override;

	void CheckInput() override;

private:
	float fullSpinTime = 0.4f;	// Time it takes to the player to draw a full circle spinning
	float spinTimer = 0.0f;		// Timer to check time performing a skill

	bool atatckStarted = false;

	// Move variables
	float duration = 1.5f;
	std::vector<float3>path;
	unsigned pathIndex = 0;
	float moveTimer = 0.0f;
};

#endif __CircularAttackSkill_h__