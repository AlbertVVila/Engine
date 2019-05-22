#include "PlayerMovement.h"

#include "Application.h"

#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleNavigation.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentAnimation.h"

#include "PlayerStateWalk.h"
#include "PlayerStateIdle.h"

#include "JSON.h"
#include <assert.h>
#include <string>
#include "imgui.h"
#include "Globals.h"
#include "debugdraw.h"

#define CLOSE_ENOUGH 400.0f

PlayerStateWalk::PlayerStateWalk(PlayerMovement* PM)
{
	player = PM;
	trigger = "Walk";
}


PlayerStateWalk::~PlayerStateWalk()
{
}

void PlayerStateWalk::Update()
{
	if (player->Appl->input->GetMouseButtonDown(3) == KEY_DOWN) //RIGHT BUTTON
	{
		math::float3 intersectionPoint = math::float3::inf;
		if (player->Appl->scene->Intersects(intersectionPoint, "floor"))
		{
			player->Appl->navigation->FindPath(gameobject->transform->position, intersectionPoint, path);
			pathIndex = 0;
		}
		else
		{
			//clicked outside of the map, stop moving
			playerWalking = false;
			return;
		}
	}
	if (path.size() > 0)
	{
		math::float3 currentPosition = gameobject->transform->GetPosition();
		while (pathIndex < path.size() && currentPosition.DistanceSq(path[pathIndex]) < CLOSE_ENOUGH)
		{
			pathIndex++;
		}
		if (pathIndex < path.size())
		{
			gameobject->transform->LookAt(path[pathIndex]);
			math::float3 direction = (path[pathIndex] - currentPosition).Normalized();
			gameobject->transform->SetPosition(currentPosition + player->walkingSpeed * direction * player->Appl->time->gameDeltaTime);
			playerWalking = true;
		}
		else
		{
			playerWalking = false;
		}
	}	
}

void PlayerStateWalk::Enter(GameObject* go)
{
	gameobject = go;
}

void PlayerStateWalk::CheckInput()
{

	if (player->IsAtacking())
	{
		player->currentState = (PlayerState*)player->firstAttack;
	}
	else if (player->IsUsingFirstSkill())
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
