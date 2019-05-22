#include "PlayerStateDash.h"
#include "PlayerMovement.h"

#include "Application.h"

#include "ModuleScene.h"
#include "ModuleNavigation.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"


#include "PlayerStateWalk.h"
#include "PlayerStateIdle.h"

#include "JSON.h"
#include <assert.h>
#include <string>
#include "imgui.h"
#include "Globals.h"
#include "debugdraw.h"

#define CLOSE_ENOUGH 400.0f

PlayerStateDash::PlayerStateDash(PlayerMovement* PM)
{
	player = PM;
	trigger = "Dash";
	boxSize = math::float3(80.f, 100.f, 200.f);
}


PlayerStateDash::~PlayerStateDash()
{
}

void PlayerStateDash::Update()
{

	if (path.size() > 0 && timer > dashPreparationTime)
	{
		math::float3 currentPosition = player->gameobject->transform->GetPosition();
		while (pathIndex < path.size() && currentPosition.DistanceSq(path[pathIndex]) < CLOSE_ENOUGH)
		{
			pathIndex++;
		}
		if (pathIndex < path.size())
		{
			player->gameobject->transform->LookAt(path[pathIndex]);
			math::float3 direction = (path[pathIndex] - currentPosition).Normalized();
			player->gameobject->transform->SetPosition(currentPosition + dashSpeed * direction * player->Appl->time->gameDeltaTime);
		}
	}

	if (player->boxTrigger != nullptr && !hitboxCreated && timer > player->dashDuration * minTime && timer < player->dashDuration * maxTime)
	{
		//Create the hitbox
		player->boxTrigger->SetBoxSize(boxSize);
		boxPosition = player->transform->up *100.f; //this front stuff isnt working well when rotating the chicken
		player->boxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
		hitboxCreated = true;
	}
	if (player->boxTrigger != nullptr &&hitboxCreated && timer > player->dashDuration * maxTime)
	{
		player->boxTrigger->SetBoxSize(1, 1, 1);
		hitboxCreated = false;
	}
}

void PlayerStateDash::Enter()
{
	if (player->Appl->scene->Intersects(intersectionPoint, "floor"))
	{
		player->Appl->navigation->FindPath(player->gameobject->transform->position, intersectionPoint, path);
		pathIndex = 0;
		player->gameobject->transform->LookAt(intersectionPoint);
	}
}

void PlayerStateDash::CheckInput()
{
	if (timer > player->dashDuration) // can switch?¿¿?
	{
		if (player->IsAtacking())
		{
			player->currentState = (PlayerState*)player->firstAttack;
		}
		else if (player->IsUsingFirstSkill()) //cooldown?
		{
			player->currentState = (PlayerState*)player->dash;
		}
		else if (player->IsUsingSecondSkill())
		{
			player->currentState = (PlayerState*)player->uppercut;
		}
		else if (player->IsMoving())
		{
			player->currentState = (PlayerState*)player->walk;
		}
		else
		{
			player->currentState = (PlayerState*)player->idle;
		}
	}
}
