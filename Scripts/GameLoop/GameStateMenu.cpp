#include "GameStateMenu.h"

#include "GameLoop.h"

#include "GameObject.h"
#include "ComponentButton.h"


#define GRAVEYARD_SCENE "Level0-TheGraveyard"

GameStateMenu::GameStateMenu(GameLoop* GL) : GameState(GL)
{
}


GameStateMenu::~GameStateMenu()
{
}

void GameStateMenu::Update()
{
	if (((Button*)(gLoop->menuButtons[0]))->IsPressed()) //PlayButton
	{
		gLoop->gameState = (GameState*)gLoop->loadingState;
		gLoop->menu->SetActive(false);
		gLoop->loadingGO->SetActive(true);
		gLoop->sceneToLoad = GRAVEYARD_SCENE;
	}
	else if (gLoop->optionButton->IsPressed())
	{
		gLoop->options->SetActive(true);
		gLoop->EnableMenuButtons(false);
		gLoop->gameState = (GameState*)gLoop->optionsState;
	}
	else if (gLoop->controlsButton->IsPressed())
	{
		gLoop->controls->SetActive(true);
		gLoop->EnableMenuButtons(false);
		gLoop->gameState = (GameState*)gLoop->controlsState;
	}
	else if (gLoop->creditsButton->IsPressed())
	{
		gLoop->gameState = (GameState*)gLoop->creditsState;
	}
	else if (gLoop->exitButton->IsPressed())
	{
		gLoop->gameState = (GameState*)gLoop->quitState;
	}
}
