#include "Application.h"
#include "ModuleScene.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "ComponentRenderer.h"
#include "ComponentAnimation.h"
#include "ComponentTransform.h"

#include "BossStateSecondInterPhase.h"
#include "EnemyControllerScript/EnemyControllerScript.h"

#include "BossBehaviourScript.h"

BossStateSecondInterPhase::BossStateSecondInterPhase(BossBehaviourScript* AIboss)
{
	boss = AIboss;
}


BossStateSecondInterPhase::~BossStateSecondInterPhase()
{
}

void BossStateSecondInterPhase::HandleIA()
{
	//this will have a fixed duration
	if (finished)
	{
		boss->currentState = (BossState*)boss->thirdIdle;
	}
}

void BossStateSecondInterPhase::Update()
{
	//Update parts

	switch (state)
	{
		case InterphaseState::None:
			state = InterphaseState::Kneel;
			boss->enemyController->anim->SendTriggerToStateMachine("Kneel");
			boss->secondInterphaseKneelTime = boss->enemyController->anim->GetDurationFromClip();
			break;
		case InterphaseState::Kneel:
			if (kneelTimer > boss->secondInterphaseKneelTime)
			{
				state = InterphaseState::Cry;
				boss->enemyController->anim->SendTriggerToStateMachine("Cry");

			}
			else
			{
				kneelTimer += boss->App->time->gameDeltaTime;
			}
			break;
		case InterphaseState::Cry:
			if (cryTimer > boss->secondInterphaseCryTime)
			{
				state = InterphaseState::FadeOff;
			}
			else
			{
				cryTimer += boss->App->time->gameDeltaTime;
			}
			break;
		case InterphaseState::FadeOff:

			if (fadeOffTimer / boss->secondInterphaseFadeOffTime < 1.0f)
			{
				for (auto render : boss->enemyController->myRenders)
				{
					render->dissolveAmount = fadeOffTimer / boss->secondInterphaseFadeOffTime;
				}	
			}
			else
			{
				state = InterphaseState::FloorVanish;
			}

			fadeOffTimer += boss->App->time->gameDeltaTime;
			break;
		case InterphaseState::FloorVanish:
			//just vanish everything
			//I need to load a vector with all the renders..
			if (floorVanishTimer / boss->secondInterphaseFloorVanishTime < 1.0f)
			{
				for (auto vanishGO : boss->secondInterphaseVanishGOs)
				{
					vanishGO->GetComponent<ComponentRenderer>()->dissolveAmount = floorVanishTimer / boss->secondInterphaseFloorVanishTime;
				}
				//for (auto flameGO : boss->secondInterphaseFlameGOs)
				//{
				//	float newScale = 
				//}
			}
			else
			{
				state = InterphaseState::Teleport;
			}

			floorVanishTimer += boss->App->time->gameDeltaTime;

			//state = InterphaseState::Teleport;

			break;
		case InterphaseState::Teleport:

			boss->enemyController->SetPosition(boss->secondInterphaseFinalPosition);
			boss->gameobject->transform->SetScale(math::float3(boss->secondInterphaseFinalScale, boss->secondInterphaseFinalScale, boss->secondInterphaseFinalScale));
			boss->enemyController->anim->SendTriggerToStateMachine("ThirdIdle");
			state = InterphaseState::FadeIn;
			break;
		case InterphaseState::FadeIn:

			if (fadeInTimer / boss->secondInterphaseFadeInTime < 1.0f)
			{
				for (auto render : boss->enemyController->myRenders)
				{
					render->dissolveAmount = 1 - (fadeInTimer / boss->secondInterphaseFadeInTime);
				}

			}
			else
			{
				state = InterphaseState::Finished;
			}

			fadeInTimer += boss->App->time->gameDeltaTime;
			
			break;
		case InterphaseState::Finished:
			finished = true;
			break;
	}
	//Make her kneel

	//Disappear

	//Throne floor and everything on top needs to disappear. no need to touch the navmesh

	//Teleport her to the designated spot and change her scale

	//change the animation and make her appear again

	//maybe wait a bit idk
}

void BossStateSecondInterPhase::Enter()
{
	//We need to reset variables and get everything ready
	boss->ResetVariables();
}

void BossStateSecondInterPhase::Exit()
{

}
