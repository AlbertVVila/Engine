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

PlayerStateWalk::PlayerStateWalk(PlayerMovement* PM, const char* trigger):
	PlayerState(PM, trigger)
{
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
			player->Appl->navigation->FindPath(player->gameobject->transform->position, intersectionPoint, path);
			pathIndex = 0;
		}
		else
		{
			//clicked outside of the map, stop moving
			playerWalking = false;
			if (dustParticles)
			{
				dustParticles->SetActive(false);
			}
			return;
		}
	}
	if (path.size() > 0)
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
			player->gameobject->transform->SetPosition(currentPosition + player->walkingSpeed * direction * player->Appl->time->gameDeltaTime);
			playerWalking = true;
			if (dustParticles)
			{
				dustParticles->SetActive(true);
			}
		}
		else
		{
			playerWalking = false;
			if (dustParticles)
			{
				dustParticles->SetActive(false);
			}
		}
	}	
}

void PlayerStateWalk::Enter()
{
	if (dustParticles)
	{
		dustParticles->SetActive(true);
	}
}

void PlayerStateWalk::CheckInput()
{

	if (player->IsAtacking())
	{
		player->currentState = (PlayerState*)player->firstAttack;
		if (dustParticles)
		{
			dustParticles->SetActive(false);
		}
	}
	else if (player->IsUsingFirstSkill())
	{
		player->currentState = (PlayerState*)player->dash;
		if (dustParticles)
		{
			dustParticles->SetActive(false);
		}
	}
	else if (player->IsUsingSecondSkill())
	{
		player->currentState = (PlayerState*)player->uppercut;
		if (dustParticles)
		{
			dustParticles->SetActive(false);
		}
	}
	else if (player->IsMoving())
	{
		player->currentState = (PlayerState*)player->walk;
	}
	else
	{
		player->currentState = (PlayerState*)player->idle;
		if (dustParticles)
		{
			dustParticles->SetActive(false);
		}
	}
}
