#include "RainSkill.h"

#include "PlayerMovement.h"

#include "ModuleNavigation.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentRenderer.h"
#include "ResourceMaterial.h"

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
	if (!machetes.empty())
	{
		LOG("Machetes placed");
		decal->SetActive(false);
		for (unsigned i = 0u; i < MACHETE_AMOUNT; ++i)
		{
			GameObject* machete = machetes[i].machete;
			const float MIN_RAND = -AREA_SIZE, MAX_RAND = AREA_SIZE;
			const float range = MAX_RAND - MIN_RAND;
			float x = range * ((((float)rand()) / (float)RAND_MAX)) + MIN_RAND;			
			float z = range * ((((float)rand()) / (float)RAND_MAX)) + MIN_RAND;			
			machete->transform->SetGlobalPosition(spawnPosition + float3(x, i * VERTICAL_OFFSET, z));
			player->macheteRainActivated = true;
			targetHeight = spawnPosition.y - MACHETE_RAIN_START_HEIGHT;
			machetes[i].landed = false;
			machete->transform->scale = machetes[i].originalScale;
			machete->UpdateTransforms(math::float4x4::identity);
			machetes[i].colliderFrames = 5u;
		}
	}
	
	RangeSkill::Start();
}

void RainSkill::Prepare()
{
	math::float3 mousePosition;
	if (player->App->navigation->FindIntersectionPoint(player->transform->GetPosition(), mousePosition))
	{
		spawnPosition = mousePosition + float3(0.f, MACHETE_RAIN_START_HEIGHT, 0.f);
	}
	else
	{
		spawnPosition = player->transform->position + float3(0.f, MACHETE_RAIN_START_HEIGHT, 0.f);
	}
	math::float3 groundSpawnPos = spawnPosition - float3(0.f, MACHETE_RAIN_START_HEIGHT * .98f, 0.f);
	decal->transform->SetGlobalPosition(groundSpawnPos);
	decal->SetActive(true);
	if (groundSpawnPos.Distance(player->transform->GetGlobalPosition()) < MACHETE_SKILL_RANGE)
	{
		decalMaterial->diffuseColor = decalOriginalColor;
		canceled = false;
	}
	else
	{
		decalMaterial->diffuseColor = decalOriginalColor * .01f;
		decalMaterial->diffuseColor.w = decalOriginalColor.w;
		canceled = true;
	}
}

bool RainSkill::OnCancel()
{
	if (canceled)
	{
		decal->SetActive(false);
		player->currentState = (PlayerState*)player->idle;
		return true;
	}
	return false;
}

