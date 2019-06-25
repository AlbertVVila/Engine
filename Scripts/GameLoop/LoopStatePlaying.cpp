#include "LoopStatePlaying.h"

#include "GameLoop.h"

#include "GameObject.h"
#include "ComponentButton.h"
#include "ComponentTransform2D.h"

#include "ModuleScene.h"
#include "ModuleInput.h"
#include "Math/float2.h"
#include "PlayerMovement.h"

#define MENU_SCENE "MenuScene"
#define TEMPLE_SCENE "Level2-TheForbiddenTempleV1"

LoopStatePlaying::LoopStatePlaying(GameLoop* GL) : LoopState(GL)
{
}


LoopStatePlaying::~LoopStatePlaying()
{
}

void LoopStatePlaying::HandleHotkeys()
{
	/*if (gLoop->App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		gLoop->closePlayerMenuButton->isKeyUp = true;
	}
	else if ()
	{
		gLoop->inventoryButton->isKeyDown = true;
	}
	else if (gLoop->App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
	{
		gLoop->missionsButton->isKeyDown = true;
	}
	else if (gLoop->App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
	{
		gLoop->skillsButton->isKeyDown = true;
	}
	else if (gLoop->App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		//This will open the map
	}*/
}

void LoopStatePlaying::Update()
{
	if (gLoop->hudBackToMenuButton->IsPressed()) LoadMainMenu();

	if (gLoop->closePlayerMenuButton->KeyUp()) CloseMenu();
	if (gLoop->playerMenuGO->isActive() && gLoop->App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) CloseMenu();

	if (gLoop->inventoryButton->IsPressed() || gLoop->App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN)
	{
		if (gLoop->inventoryMenuGO->isActive()) CloseMenu();
		else OpenMenu(gLoop->inventoryMenuGO);
	}

	if (gLoop->skillsButton->IsPressed() || gLoop->App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
	{
		if (gLoop->skillsMenuGO->isActive()) CloseMenu();
		else OpenMenu(gLoop->skillsMenuGO);
	}

	if (gLoop->missionsButton->IsPressed() || gLoop->App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
	{
		if (gLoop->missionsMenuGO->isActive()) CloseMenu();
		else OpenMenu(gLoop->missionsMenuGO);
	}

	if (gLoop->playerScript->isPlayerDead)
	{
		gLoop->loseWindow->SetActive(true);
		gLoop->currentLoopState = (LoopState*)gLoop->deadState;
	}
	else if (gLoop->winBbox->Intersects(*(gLoop->playerBbox)))
	{
		if (gLoop->gameScene == GameScene::CEMENTERY)
		{
			gLoop->currentLoopState = (LoopState*)gLoop->loadingState;
			gLoop->loadingGO->SetActive(true);
			gLoop->playerMenuGO->SetActive(false);
			gLoop->sceneToLoad = TEMPLE_SCENE;
			gLoop->App->scene->actionAfterLoad = true;
			gLoop->App->scene->stateAfterLoad = "Temple";
			gLoop->stateAfterLoad = (LoopState*)gLoop->creditsState;
			gLoop->gameScene == GameScene::TEMPLE;
			
		}
		else if (gLoop->gameScene == GameScene::TEMPLE)
		{
			gLoop->winWindow->SetActive(true);
			gLoop->currentLoopState = (LoopState*)gLoop->winState;
			gLoop->gameScene == GameScene::MENU;
		}
	}
}

void LoopStatePlaying::LoadMainMenu()
{
	gLoop->currentLoopState = (LoopState*)gLoop->loadingState;
	gLoop->playerMenuGO->SetActive(false);
	gLoop->hudGO->SetActive(false);
	gLoop->loadingGO->SetActive(true);
	gLoop->sceneToLoad = MENU_SCENE;
}

void LoopStatePlaying::OpenMenu(GameObject * menu)
{
	gLoop->playerMenuGO->SetActive(true);
	
	gLoop->inventoryButton->rectTransform->setPosition(math::float2(-485, gLoop->inventoryButton->rectTransform->getPosition().y));
	gLoop->skillsButton->rectTransform->setPosition(math::float2(-485, gLoop->skillsButton->rectTransform->getPosition().y));
	gLoop->missionsButton->rectTransform->setPosition(math::float2(-485, gLoop->missionsButton->rectTransform->getPosition().y));
	
	gLoop->inventoryMenuGO->SetActive(menu == gLoop->inventoryMenuGO);
	gLoop->skillsMenuGO->SetActive(   menu == gLoop->skillsMenuGO);
	gLoop->missionsMenuGO->SetActive( menu == gLoop->missionsMenuGO);
}

void LoopStatePlaying::CloseMenu()
{
	gLoop->closePlayerMenuButton->isHovered = false;

	gLoop->inventoryMenuGO->SetActive(false);
	gLoop->skillsMenuGO->SetActive(false);
	gLoop->missionsMenuGO->SetActive(false);
	gLoop->playerMenuGO->SetActive(false);

	gLoop->inventoryButton->rectTransform->setPosition(math::float2(-50, gLoop->inventoryButton->rectTransform->getPosition().y));
	gLoop->skillsButton->rectTransform->setPosition(math::float2(-50, gLoop->skillsButton->rectTransform->getPosition().y));
	gLoop->missionsButton->rectTransform->setPosition(math::float2(-50, gLoop->missionsButton->rectTransform->getPosition().y));
}
