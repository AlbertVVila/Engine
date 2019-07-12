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

#include "PlayerStateWalkToHitEnemy.h"
#include "PlayerStateIdle.h"

#include "BasicSkill.h"

#include "JSON.h"
#include <assert.h>
#include <string>
#include "imgui.h"
#include "Globals.h"
#include "debugdraw.h"

#define RECALC_PATH_TIME 0.3f

PlayerStateWalkToHitEnemy::PlayerStateWalkToHitEnemy(PlayerMovement* PM, const char* trigger):
	PlayerState(PM, trigger)
{
}

PlayerStateWalkToHitEnemy::~PlayerStateWalkToHitEnemy()
{
}

void PlayerStateWalkToHitEnemy::Update()
{
	//if enemy moved, re calc path towards it
	if (enemyTargeted->transform->position != enemyPosition)
	{
		//reset stuff
		path.clear();
		enemyPosition = enemyTargeted->transform->position;
		//re calculate path
		if (player->App->navigation->FindPath(player->gameobject->transform->position, enemyPosition,
			path, PathFindType::FOLLOW, math::float3(0, player->OutOfMeshCorrectionY, 0), defaultMaxDist, player->straightPathingDistance))
		{
			pathIndex = 0;
		}
		//if error
		else
		{
			LOG("Error walking to hit enemy along the way");
			playerWalking = false;
			player->currentState = player->idle;
			if (dustParticles)
			{
				dustParticles->SetActive(false);
			}
			return;
		}
	}
	//maybe we need to check if still clicking on the enemy
	/*
	if (player->App->input->GetMouseButtonDown(1) == KEY_DOWN 
		|| player->App->input->GetMouseButtonDown(1) == KEY_REPEAT)
	{
		
		
	}
	else if (player->App->input->GetMouseButtonDown(1) == KEY_REPEAT)
	{
		moveTimer += player->App->time->gameDeltaTime;
	}*/

	if (path.size() > 0)
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
			
			player->currentSkill = player->allSkills[player->assignedSkills[HUB_BUTTON_RC]]->skill;

			SkillType skillType = player->allSkills[player->assignedSkills[HUB_BUTTON_RC]]->type;

			//entering code
			{

				player->currentState = (PlayerState*)player->attack;

				// Play skill animation
				if (player->anim != nullptr)
				{
					player->anim->SendTriggerToStateMachine(player->currentSkill->animTrigger.c_str());
				}

				player->currentSkill->duration = player->anim->GetDurationFromClip();

				player->UseSkill(skillType);
				player->currentSkill->Start();
			}
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

void PlayerStateWalkToHitEnemy::Enter()
{
	if (dustParticles)
	{
		dustParticles->SetActive(true);
		player->anim->controller->current->speed *= (1 + (player->stats.dexterity * 0.005f));
	}
	if (player->App->scene->enemyHovered != nullptr)
	{
		enemyTargeted = player->App->scene->enemyHovered;
		enemyPosition = enemyTargeted->transform->position;
		moveTimer = 0.0f;
		math::float3 intPos(0.f, 0.f, 0.f);
		if (player->App->navigation->FindPath(player->gameobject->transform->position, enemyPosition,
			path, PathFindType::FOLLOW, math::float3(0, player->OutOfMeshCorrectionY, 0), defaultMaxDist, player->straightPathingDistance))
		{
			//case the player clicks outside of the floor mesh but we want to get close to the floors edge
			pathIndex = 0;
			return;
		}
	}
	//something went wrong, stop moving
	LOG("Error walking to hit enemy");
	playerWalking = false;
	player->currentState = player->idle;
	if (dustParticles)
	{
		dustParticles->SetActive(false);
	}
	return;
}
