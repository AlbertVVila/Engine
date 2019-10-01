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

			doorLerpTimer += boss->App->time->gameDeltaTime;

			firstLambda = CalculateDoorLambda();
			SetPlayerCameraPosition(boss->InterpolateFloat3(cameraResetPosition, boss->cameraPositionDoorCS, firstLambda)); 
			SetPlayerCameraRotation(boss->InterpolateQuat(cameraResetRotation, boss->cameraRotationDoorCS, firstLambda));

			if (doorLerpTimer >= boss->cutsceneDoorDuration)
			{
				csState = CutsceneState::DoorClosing;
			}
			break;

		case CutsceneState::DoorClosing:

			doorClosingTimer += boss->App->time->gameDeltaTime;

			if (doorClosingTimer >= boss->cutsceneDoorRisingDuration)
			{
				csState = CutsceneState::BossLerping;
			}
			break;

		case CutsceneState::BossLerping:

			bossLerpTimer += boss->App->time->gameDeltaTime;

			secondLambda = CalculateBossLambda();
			SetPlayerCameraPosition(boss->InterpolateFloat3(boss->cameraPositionDoorCS, boss->cameraPositionBossCS, secondLambda));
			SetPlayerCameraRotation(boss->InterpolateQuat(boss->cameraRotationDoorCS, boss->cameraRotationBossCS, secondLambda));

			if (doorLerpTimer >= boss->cutsceneBossDuration)
			{
				csState = CutsceneState::PlayerLerping;
			}
			break;

		case CutsceneState::PlayerLerping:

			playerLerpTimer += boss->App->time->gameDeltaTime;

			thirdLambda = CalculatePlayerLambda();
			SetPlayerCameraPosition(boss->InterpolateFloat3(boss->cameraPositionBossCS, cameraResetPosition, thirdLambda));
			SetPlayerCameraRotation(boss->InterpolateQuat(boss->cameraRotationBossCS, cameraResetRotation, thirdLambda));

			if (playerLerpTimer >= boss->cutsceneBackToPlayerDuration)
			{
				csState = CutsceneState::Finished;
			}

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
	return (doorLerpTimer + boss->App->time->gameDeltaTime) / boss->cutsceneDoorDuration;
}

float BossStateCutScene::CalculateBossLambda()
{
	return (bossLerpTimer + boss->App->time->gameDeltaTime) / boss->cutsceneBossDuration;
}

float BossStateCutScene::CalculatePlayerLambda()
{
	return (playerLerpTimer + boss->App->time->gameDeltaTime) / boss->cutsceneBackToPlayerDuration;
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