#include "DashSkill.h"

#include "Application.h"
#include "ModuleNavigation.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"

#include "PlayerMovement.h"
#include "PlayerStateAttack.h"

DashSkill::DashSkill(PlayerMovement* PM, const char* trigger, ComponentBoxTrigger* attackBox) :
	MeleeSkill(PM, trigger, attackBox)
{
}

DashSkill::~DashSkill()
{
}

void DashSkill::Start()
{
	BasicSkill::Start();
	player->attack->trigger = "Dash";

	if (player->App->navigation->NavigateTowardsCursor(player->gameobject->transform->position, path,
		math::float3(player->OutOfMeshCorrectionXZ, player->OutOfMeshCorrectionY, player->OutOfMeshCorrectionXZ), intersectionPoint))
	{
		pathIndex = 0;
		player->gameobject->transform->LookAt(intersectionPoint);
		if (dashFX)
		{
			dashFX->SetActive(true);
		}
		if (dashMesh)
		{
			dashMesh->SetActive(true);
			dashMesh->transform->scale = meshOriginalScale;
			dashMesh->transform->Scale(1.0f);
			scalator = originalScalator;
		}
		player->ResetCooldown(HUB_BUTTON_Q);
	}
}

void DashSkill::UseSkill()
{

	if (path.size() > 0 && timer > dashPreparationTime)
	{
		math::float3 currentPosition = player->gameobject->transform->GetPosition();
		while (pathIndex < path.size() && currentPosition.DistanceSq(path[pathIndex]) < MINIMUM_PATH_DISTANCE)
		{
			pathIndex++;
		}
		if (pathIndex < path.size())
		{
			player->gameobject->transform->LookAt(path[pathIndex]);
			math::float3 direction = (path[pathIndex] - currentPosition).Normalized();
			player->gameobject->transform->SetPosition(currentPosition + dashSpeed * direction * player->App->time->gameDeltaTime);
			if (dashMesh)
			{
				dashMesh->transform->Scale(scalator);
				scalator -= scalatorDecay;
				scalator = MAX(1.0f, scalator);
			}
		}
	}

	if (player->attackBoxTrigger != nullptr && !hitboxCreated && timer > duration)
	{
		//Create the hitbox
		player->attackBoxTrigger->Enable(true);
		player->attackBoxTrigger->SetBoxSize(boxSize);
		boxPosition = player->transform->up *100.f; //this front stuff isnt working well when rotating the chicken
		player->attackBoxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
		hitboxCreated = true;
	}
	if (player->attackBoxTrigger != nullptr &&hitboxCreated && timer > duration)
	{
		player->attackBoxTrigger->Enable(false);
		hitboxCreated = false;
	}
}

void DashSkill::Reset()
{
	MeleeSkill::Reset();

	if (attackBoxTrigger != nullptr)
	{
		attackBoxTrigger->Enable(false);
		hitboxCreated = false;
	}

	if (dashFX != nullptr)
	{
		dashFX->SetActive(false);
	}
	if (dashMesh != nullptr)
	{
		dashMesh->SetActive(false);
	}

	// Play animation
}