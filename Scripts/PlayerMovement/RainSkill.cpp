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
		spawnPosition = mousePosition + float3(0.f, MACHETE_RAIN_START_HEIGHT, 0.f);
	}
	else
	{
		spawnPosition = player->transform->position + float3(0.f, MACHETE_RAIN_START_HEIGHT, 0.f);
	}
	
	if (!machetes.empty())
	{
		LOG("Machetes placed");
		for (unsigned i = 0u; i < MACHETE_AMOUNT; ++i)
		{
			GameObject* machete = machetes.front();
			const float MIN_RAND = -400.0f, MAX_RAND = 400.0f;
			const float range = MAX_RAND - MIN_RAND;
			float x = range * ((((float)rand()) / (float)RAND_MAX)) + MIN_RAND;			
			float z = range * ((((float)rand()) / (float)RAND_MAX)) + MIN_RAND;			
			machete->transform->SetGlobalPosition(spawnPosition + float3(x, i * 1000.f, z));
			player->macheteRainActivated = true;
			player->macheteRainParticles->SetActive(true);
			targetHeight = spawnPosition.y - MACHETE_RAIN_START_HEIGHT;
			machetes.push(machete);
			machetes.pop();
		}
	}
	
	RangeSkill::Start();
}

