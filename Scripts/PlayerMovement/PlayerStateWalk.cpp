#include "PlayerMovement.h"

#include "Application.h"

#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleNavigation.h"
#include "ModuleTime.h"
#include "ModuleWindow.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentAnimation.h"

#include "PlayerStateWalk.h"
#include "PlayerStateIdle.h"

#include "BasicSkill.h"

#include "JSON.h"
#include <assert.h>
#include <string>
#include "imgui.h"
#include "Globals.h"
#include "debugdraw.h"


#define RECALC_PATH_TIME 0.3f

PlayerStateWalk::PlayerStateWalk(PlayerMovement* PM, const char* trigger):
	PlayerState(PM, trigger)
{
}

PlayerStateWalk::~PlayerStateWalk()
{
}

void PlayerStateWalk::lerpCalculations(const math::float3& direction)
{
	math::float3 playerFront = -player->gameobject->transform->front;
	float angleResult = direction.AngleBetweenNorm(-player->gameobject->transform->front.Normalized());

	//block of code to check how much the direction has to be changed to look at the next point
	{
		//0.26 rad is 15 deg, if player looking 15 deg or less off, we just look at the walking direction
		if (angleResult < 0.26f && angleResult > -0.26f)
		{
			player->gameobject->transform->LookAt(path[pathIndex]);
			currentLerping = 0.00;
		}
		//case the difference is bigger, the player lerps
		else
		{
			//getting the values in 2D avoids so much trobule
			//we get the starting and final direction in 2D
			math::float2 startingDirection = math::float2(-player->gameobject->transform->front.Normalized().x, -player->gameobject->transform->front.Normalized().z);
			math::float2 finalDirection = math::float2(direction.x, direction.z);

			//calculate the current direction using the Lerp function
			math::float2 directionToLook2D = startingDirection.Lerp(finalDirection, currentLerping);
			//set things together to calculate the Y value using the Dot product (perpendicular of x and z)
			math::float3 directionToLookX(directionToLook2D.x, 0, 0);
			math::float3 directionToLookZ(0, 0, directionToLook2D.y);
			//get the desired direction
			math::float3 directionToLook3D = math::float3(RadToDeg(directionToLook2D.x), directionToLookZ.Dot(directionToLookX), RadToDeg(directionToLook2D.y));

			//look at the point
			player->gameobject->transform->LookAt(player->gameobject->transform->GetGlobalPosition() + directionToLook3D);
			//increment lerping value for the next iteration taking into consideration both the delta time and the current angle
			//we take into consideration the angle because if the player is to rotate 180 degrees, it takes a little too long
			currentLerping += lerpingIncrement * player->App->time->gameDeltaTime * angleResult;
		}
	}
}

void PlayerStateWalk::Update()
{
	math:float2 mouse((float*)&player->App->input->GetMousePosition());
	if (player->App->input->GetMouseButtonDown(1) == KEY_DOWN 
		|| player->App->input->GetMouseButtonDown(1) == KEY_REPEAT)
	{
		moveTimer = 0.0f;
		math::float3 intPos(0.f, 0.f, 0.f);
		if (player->App->navigation->NavigateTowardsCursor(player->gameobject->transform->position, path,
					math::float3(player->OutOfMeshCorrectionXZ, player->OutOfMeshCorrectionY, player->OutOfMeshCorrectionXZ), 
					intPos, 10000, PathFindType::FOLLOW, player->straightPathingDistance))
		{
			//case the player clicks outside of the floor mesh but we want to get close to the floors edge
			pathIndex = 0;
		}
		else
		{
			//clicked outside of the map, stop moving
			playerWalking = false;
			player->currentState = player->idle;
			if (dustParticles)
			{
				dustParticles->SetActive(false);
			}
			return;
		}
	}
	else if (player->App->input->GetMouseButtonDown(1) == KEY_REPEAT)
	{
		moveTimer += player->App->time->gameDeltaTime;
	}
	if (path.size() > 0)
	{
		math::float3 currentPosition = player->gameobject->transform->GetPosition();
		while (pathIndex < path.size() && currentPosition.DistanceSq(path[pathIndex]) < MINIMUM_PATH_DISTANCE)
		{
			pathIndex++;
		}
		if (pathIndex < path.size())
		{
			
			math::float3 direction = (path[pathIndex] - currentPosition).Normalized();
			lerpCalculations(direction);
			
			math::float3 finalWalkingSpeed = player->walkingSpeed * direction * player->App->time->gameDeltaTime;
			finalWalkingSpeed *= (1 + (player->stats.dexterity * 0.005f));
			player->gameobject->transform->SetPosition(currentPosition + finalWalkingSpeed);
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
	else
	{
		player->currentState = player->idle;
	}
}

void PlayerStateWalk::Enter()
{
	if (dustParticles)
	{
		dustParticles->SetActive(true);
		player->anim->controller->current->speed *= (1 + (player->stats.dexterity * 0.005f));
	}
}

void PlayerStateWalk::CheckInput()
{

	/*if (player->IsAtacking())
	{
		//player->currentState = (PlayerState*)player->firstAttack;
		if (dustParticles)
		{
			dustParticles->SetActive(false);
		}
	}
	else if (player->IsUsingFirstSkill())
	{
		player->currentState = player->allSkills[player->activeSkills[0]]->state;
		if (dustParticles)
		{
			dustParticles->SetActive(false);
		}
		if (dustParticles)
		{
			dustParticles->SetActive(false);
		}
	}
	else if (player->IsUsingSecondSkill())
	{
		player->currentState = player->allSkills[player->activeSkills[1]]->state;
		if (dustParticles)
		{
			dustParticles->SetActive(false);
		}
	}*/
	if (player->IsUsingSkill() || player->IsAttacking())
	{
		player->currentState = (PlayerState*)player->attack;
	}
	else if (player->IsMovingToAttack())
	{
		player->currentState = (PlayerState*)player->walkToHit;
	}
	else if (player->IsMovingToItem())
	{
		player->currentState = (PlayerState*)player->walkToPickItem;
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
