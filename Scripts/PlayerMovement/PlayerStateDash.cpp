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

	if (!hitboxCreated && timer > player->dashDuration * minTime && timer < player->dashDuration * maxTime)
	{
		//Create the hitbox
		player->boxTrigger->SetBoxSize(boxSize);
		boxPosition = player->transform->up *100.f; //this front stuff isnt working well when rotating the chicken
		player->boxTrigger->SetBoxPosition(boxPosition.x, boxPosition.y, boxPosition.z + 100.f);
		hitboxCreated = true;
	}
	if (hitboxCreated && timer > player->dashDuration * maxTime)
	{
		player->boxTrigger->SetBoxSize(1, 1, 1);
		hitboxCreated = false;
	}

	//math::float3 intersectionPoint = math::float3::inf;
	//if (player->Appl->scene->Intersects(intersectionPoint, "floor"))
	//{
	//	player->Appl->navigation->FindPath(player->gameobject->transform->position, intersectionPoint, player->path);
	//	player->pathIndex = 0;
	//}
	//if (player->path.size() > 0)
	//{
	//	math::float3 currentPosition = player->gameobject->transform->GetPosition();
	//	while (player->pathIndex < player->path.size() && currentPosition.DistanceSq(player->path[player->pathIndex]) < CLOSE_ENOUGH)
	//	{
	//		player->pathIndex++;
	//	}
	//	if (player->pathIndex < player->path.size())
	//	{
	//		player->gameobject->transform->LookAt(player->path[player->pathIndex]);
	//		math::float3 direction = (player->path[player->pathIndex] - currentPosition).Normalized();
	//		player->gameobject->transform->SetPosition(currentPosition + player->dashSpeed*direction*player->Appl->time->gameDeltaTime);
	//	}
	//}
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
