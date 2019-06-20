#include "SliceSkill.h"

#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"

#include "PlayerMovement.h"

#include "imgui.h"
#include "JSON.h"

SliceSkill::SliceSkill(PlayerMovement* PM, ComponentBoxTrigger* attackBox) :
	MeleeSkill(PM, attackBox)
{
}

SliceSkill::~SliceSkill()
{
}

void SliceSkill::Start()
{
	boxSize = math::float3(150.f, 100.f, 100.f);
}

void SliceSkill::Expose(ImGuiContext* context)
{
	MeleeSkill::Expose(context);
}

void SliceSkill::Serialize(JSON_value* json) const
{
	assert(json != nullptr);

}

void SliceSkill::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);

}

void SliceSkill::UseSkill()
{
	if (attackBoxTrigger != nullptr)
	{
		//Create the hitbox
		attackBoxTrigger->Enable(true);
		attackBoxTrigger->SetBoxSize(boxSize);
		boxPosition = player->transform->up *100.f; //this front stuff isnt working well when rotating the chicken
		attackBoxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
		hitboxCreated = true;
	}
}

void SliceSkill::Reset()
{
	MeleeSkill::Reset();

	if (attackBoxTrigger != nullptr)
	{
		attackBoxTrigger->Enable(false);
		hitboxCreated = false;
	}

	// Play animation
}