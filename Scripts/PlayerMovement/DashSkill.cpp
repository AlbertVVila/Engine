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
	MeleeSkill::Start();

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
	//Create the hitbox
	boxSize = math::float3(80.f, 100.f, 200.f);
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

	if (player->attackBoxTrigger != nullptr && !player->attackBoxTrigger->enabled)
	{
		//Update the hitbox
		boxPosition = player->transform->up *100.f; //this front stuff isnt working well when rotating the chicken
		player->attackBoxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
	}
}

void DashSkill::Reset()
{
	MeleeSkill::Reset();

	if (dashFX != nullptr)
	{
		dashFX->SetActive(false);
	}
	if (dashMesh != nullptr)
	{
		dashMesh->SetActive(false);
	}
}

void DashSkill::CheckInput()
{
	if (timer > player->currentState->duration) //CAN SWITCH?
	{

		if (player->IsUsingSkill())
		{
			player->currentState = (PlayerState*)player->attack;
		}
		else if (player->IsMoving())
		{
			player->currentState = (PlayerState*)player->walk;
		}
	}
}