#include "SliceSkill.h"

#include "imgui.h"
#include "JSON.h"

SliceSkill_API Script* CreateScript()
{
	SliceSkill* instance = new SliceSkill;
	return instance;
}

void SliceSkill::Start()
{
}

void SliceSkill::Update()
{

}

void SliceSkill::Expose(ImGuiContext* context)
{
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
}
