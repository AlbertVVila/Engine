#include "RangeSkill.h"

#include "ModuleScene.h"

#include "PlayerMovement.h"

RangeSkill::RangeSkill(PlayerMovement* PM, const char* trigger, const char* prefab) :
	BasicSkill(PM, trigger), prefabName(prefab)
{
}


RangeSkill::~RangeSkill()
{
}

void RangeSkill::Start()
{
	BasicSkill::Start();
	player->App->scene->Spawn(prefabName.c_str(), spawnPosition, math::Quat::identity);
}