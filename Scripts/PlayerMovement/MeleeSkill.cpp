#include "MeleeSkill.h"

#include "ComponentBoxTrigger.h"

#include "PlayerMovement.h"

MeleeSkill::MeleeSkill(PlayerMovement* PM, ComponentBoxTrigger* attackBox) :
	BaseSkill(PM), attackBoxTrigger(attackBox)
{
}

MeleeSkill::~MeleeSkill()
{
}

void MeleeSkill::Expose(ImGuiContext* context)
{
	BaseSkill::Expose(context);
}

void MeleeSkill::Serialize(JSON_value* json) const
{
	BaseSkill::Serialize(json);
}

void MeleeSkill::DeSerialize(JSON_value* json)
{
	BaseSkill::DeSerialize(json);
}

void MeleeSkill::Reset()
{
	BaseSkill::Reset();
}