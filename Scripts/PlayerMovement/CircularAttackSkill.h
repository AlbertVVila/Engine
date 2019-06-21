#ifndef  __CircularAttackSkill_h__
#define  __CircularAttackSkill_h__

#include "MeleeSkill.h"
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

private:
	float fullSpinTime = 0.4f;	// Time it takes to the player to draw a full circle spinning
	float spinTimer = 0.0f;		// Timer to check time performing a skill

	bool atatckStarted = false;
};

#endif __CircularAttackSkill_h__