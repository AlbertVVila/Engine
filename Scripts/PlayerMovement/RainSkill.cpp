#include "RainSkill.h"

#include "PlayerMovement.h"

//#include "ModuleScene.h"

//#include "GameObject.h"
#include "ComponentTransform.h"

//#include "Math/float2.h"

RainSkill::RainSkill(PlayerMovement* PM, const char* trigger, const char* prefab) :
	RangeSkill(PM, trigger, prefab)
{
}

RainSkill::~RainSkill()
{
}

void RainSkill::Start()
{
	//math::float2 mousePosition = player->gameobject->transform->GetScreenPosition();
	//math::float3 mousePosition = player->App->scene->Intersects()
	spawnPosition = player->transform->position;

	RangeSkill::Start();
}
