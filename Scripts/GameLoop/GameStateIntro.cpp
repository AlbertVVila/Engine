#include "GameStateIntro.h"

#include "Application.h"
#include "GameLoop.h"

#include "GameObject.h"
#include "ComponentButton.h"
#include "ComponentCamera.h"

#include "ModuleScene.h"

GameStateIntro::GameStateIntro(GameLoop* GL) : GameState(GL)
{
}


GameStateIntro::~GameStateIntro()
{
}

void GameStateIntro::Update()
{
	//if (!runningIntro)
	//{
	//	introCamera->SetActive(true);
	//	introScript->Start(); //TODO should start script first time it's enabled
	//	runningIntro = true;
	//}

	/*if (introScript->introDone)
	{*/
	gLoop->hudGO->SetActive(true);
	GameObject* playerCameraGO = App->scene->FindGameObjectByName("PlayerCamera");
	ComponentCamera* camera = (ComponentCamera*)playerCameraGO->GetComponent<ComponentCamera>();
	camera->SetAsMain();
	/*camera->isMainCamera = true;
	if (App->scene->maincamera != nullptr)
	{
		App->scene->maincamera->isMainCamera = false;
	}
	App->scene->maincamera = camera;*/
	/*introCamera->SetActive(false);
	runningIntro = false;
	introScript->introDone = false;
	componentIntroCamera->gameobject->transform->SetPosition(introScript->initialPosition);*/
	gLoop->gameState = (GameState*)gLoop->playingState;
	//}
}
