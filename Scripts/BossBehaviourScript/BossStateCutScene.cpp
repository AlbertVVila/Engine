#include "Application.h"

#include "ModuleTime.h"

#include "BossStateCutScene.h"

#include "BossBehaviourScript.h"
#include "EnemyControllerScript/EnemyControllerScript.h"

#include "GameObject.h"
#include "ComponentTransform.h"

BossStateCutScene::BossStateCutScene(BossBehaviourScript* AIBoss)
{
	boss = AIBoss;


}

BossStateCutScene::~BossStateCutScene()
{
}

void BossStateCutScene::HandleIA()
{
	//if finished everything change to summon army
	if (finished)
	{
		boss->currentState = (BossState*)boss->summonArmy;
	}
}

void BossStateCutScene::Update()
{
	switch (csState)
	{
		case CutsceneState::None:
			csState = CutsceneState::DoorLerping;
			break;
		case CutsceneState::DoorLerping:
			doorTimer += boss->App->time->gameDeltaTime;

			firstLambda = CalculateDoorLambda();
			SetPlayerCameraPosition(boss->InterpolateFloat3(cameraResetPosition, boss->cameraPositionDoorCS, firstLambda)); 
			SetPlayerCameraRotation(boss->InterpolateQuat(cameraResetRotation, boss->cameraRotationDoorCS, firstLambda));

			if (ReachedPosition(cameraResetPosition, boss->cameraPositionDoorCS))
			{
				csState = CutsceneState::DoorClosing;
			}

			break;
		case CutsceneState::DoorClosing:
			break;
		case CutsceneState::BossLerping:
			break;
		case CutsceneState::PlayerLerping:
			break;
		case CutsceneState::Finished:
			finished = true;
			break;
	}
	//flames?

	//do the lerping in

	//Close door

	//lerping to boss

	//back to player

}

void BossStateCutScene::Enter()
{
	cameraResetPosition = GetPlayerCameraPosition();
	cameraResetRotation = GetPlayerCameraRotation();

	firstCameraDirection = boss->cameraPositionDoorCS - cameraResetPosition;
	doorDistance = firstCameraDirection.Length();
	firstCameraSpeed = doorDistance / boss->cutsceneDoorDuration;
	firstCameraDirection.Normalize();

	secondCameraDirection = boss->cameraPositionBossCS - boss->cameraPositionDoorCS;
	bossDistance = secondCameraDirection.Length();
	secondCameraSpeed = bossDistance / boss->cutsceneBossDuration;
	secondCameraDirection.Normalize();

	//Deactivate player script
	//Deactivate camera script
}

void BossStateCutScene::Exit()
{
	//Activate player script
	//Activate camera script
}

float BossStateCutScene::CalculateDoorLambda()
{
	return (doorTimer + boss->App->time->gameDeltaTime) / boss->cutsceneDoorDuration;
}

float BossStateCutScene::CalculateBossLambda()
{
	return 0.0f;
}

float BossStateCutScene::CalculatePlayerLambda()
{
	return 0.0f;
}

math::float3 BossStateCutScene::GetPlayerCameraPosition()
{
	return boss->playerCamera->transform->GetPosition();
}

math::Quat BossStateCutScene::GetPlayerCameraRotation()
{
	return boss->playerCamera->transform->GetRotation();
}

void BossStateCutScene::SetPlayerCameraPosition(math::float3 newPosition)
{
	boss->playerCamera->transform->SetPosition(newPosition);
}

void BossStateCutScene::SetPlayerCameraRotation(math::Quat newRotation)
{
	boss->playerCamera->transform->SetRotation(newRotation);
}

bool BossStateCutScene::ReachedPosition(math::float3 initial, math::float3 final)
{
	float totalLength = (final - initial).Length();
	float traveledLength = (GetPlayerCameraPosition() - initial).Length();

	if (traveledLength >= totalLength * 0.98)
	{
		return true;
	}
	else
	{
		return false;
	}
}
