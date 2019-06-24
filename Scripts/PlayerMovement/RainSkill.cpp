#include "RainSkill.h"

#include "PlayerMovement.h"

#include "ModuleNavigation.h"

#include "GameObject.h"
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
	math::float3 mousePosition;
	if (player->App->navigation->FindIntersectionPoint(player->transform->GetPosition(), mousePosition))
	{
		spawnPosition = mousePosition;
	}
	else
	{
		spawnPosition = player->transform->position;
	}

	RangeSkill::Start();
}
