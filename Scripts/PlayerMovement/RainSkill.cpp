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
	if (machetes)
	{
		LOG("Machetes placed");
		machetes->transform->SetGlobalPosition(spawnPosition + float3(0.f, MACHETE_RAIN_START_HEIGHT, 0.f));
		player->macheteRainActivated = true;
		player->macheteRainParticles->SetActive(true);
		targetHeight = spawnPosition.y;
	}

	RangeSkill::Start();
}

