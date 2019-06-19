#include "MeleeSkill.h"

#include "ComponentBoxTrigger.h"

#include "PlayerMovement.h"

MeleeSkill::MeleeSkill(PlayerMovement* PM, ComponentBoxTrigger* attackBox) :
	BasicSkill(PM), attackBoxTrigger(attackBox)
{
}

MeleeSkill::~MeleeSkill()
{
}

void MeleeSkill::Expose(ImGuiContext* context)
{
	BasicSkill::Expose(context);
}

void MeleeSkill::Serialize(JSON_value* json) const
{
	BasicSkill::Serialize(json);
}

void MeleeSkill::DeSerialize(JSON_value* json)
{
	BasicSkill::DeSerialize(json);
}

void MeleeSkill::Reset()
{
	BasicSkill::Reset();
}