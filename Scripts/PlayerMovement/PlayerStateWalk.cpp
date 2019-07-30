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
			startingFront = player->gameobject->transform->front;
			currentLerping = 0;
			//testing logs
			{
				math::float3 direction = (path[pathIndex] - path[0]).Normalized();

				math::float3 playerFront = player->gameobject->transform->front;
				float angleResult = direction.AngleBetweenNorm(playerFront.Normalized());
				std::stringstream s;
				/*s << "Front = (" << playerFront.x << ", " << playerFront.y << ", " << playerFront.z << ")";
				LOG(s.str().c_str());

				s << "direction = (" << direction.x << ", " << direction.y << ", " << direction.z << ")";
				LOG(s.str().c_str());*/
				s << "Angle result = " << angleResult;
				LOG(s.str().c_str());
				player->gameobject->transform->LookAt(path[pathIndex]);
			}
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
			//block of code to check how much the direction has to be changed to look at the next point
			math::float3 direction = (path[pathIndex] - currentPosition).Normalized();
			{
				math::float3 playerFront = player->gameobject->transform->front;
				float angleResult = direction.AngleBetweenNorm(playerFront.Normalized());
				std::stringstream s;
				/*s << "Front = (" << playerFront.x << ", " << playerFront.y << ", " << playerFront.z << ")";
				LOG(s.str().c_str());

				s << "direction = (" << direction.x << ", " << direction.y << ", " << direction.z << ")";
				LOG(s.str().c_str());*/
				s << "Angle result = " << angleResult;
				LOG(s.str().c_str());
				player->gameobject->transform->LookAt(path[pathIndex]);

				//in case the difference is less that 30 degrees, we just rotate
				//0.52 is 30 degrees in radians
				/*if (angleResult < 0.52f && angleResult > -0.52f)
				{
					player->gameobject->transform->LookAt(path[pathIndex]);
				}
				else
				{
					player->gameobject->transform->LookAt(startingFront.Lerp(direction, currentLerping));
					currentLerping += 0.25f;
					//else we rotate 30 towards where we have to
					//case + 30�
					if (angleResult > 0)
					{
						direction.Lerp()
					}
					else
					{

					}
					math::float3 newDirectionToLookAt = ( - currentPosition);
					player->gameobject->transform->LookAt(path[pathIndex]);
				}*/
			}
			
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
