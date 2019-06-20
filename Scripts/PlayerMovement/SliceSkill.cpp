#include "SliceSkill.h"

#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"

#include "PlayerMovement.h"
#include "PlayerStateAttack.h"

#include "imgui.h"
#include "JSON.h"

SliceSkill::SliceSkill(PlayerMovement* PM, const char* trigger, ComponentBoxTrigger* attackBox) :
	MeleeSkill(PM, trigger, attackBox)
{
}

SliceSkill::~SliceSkill()
{
}

void SliceSkill::Start()
{
	BasicSkill::Start();

	// Create the hitbox
	boxSize = math::float3(150.f, 100.f, 100.f);
	attackBoxTrigger->Enable(true);
	attackBoxTrigger->SetBoxSize(boxSize);
}

void SliceSkill::UseSkill()
{
	if (attackBoxTrigger != nullptr)
	{
		// Update the hitbox
		boxPosition = player->transform->up *100.f; //this front stuff isnt working well when rotating the chicken
		attackBoxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
	}
}