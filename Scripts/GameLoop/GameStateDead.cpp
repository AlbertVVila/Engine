#include "GameStateDead.h"

#include "GameLoop.h"

#include "GameObject.h"
#include "ComponentButton.h"

#define GRAVEYARD_SCENE "Level0-TheGraveyard"

GameStateDead::GameStateDead(GameLoop* GL) : GameState(GL)
{
}


GameStateDead::~GameStateDead()
{
}

void GameStateDead::Update()
{
	if (gLoop->toTheAltarButton->IsPressed())
	{
		gLoop->loseWindow->SetActive(false);
		gLoop->loadingGO->SetActive(true);
		gLoop->playerMenuGO->SetActive(false);
		gLoop->gameState = (GameState*)gLoop->loadingState;
		gLoop->sceneToLoad = GRAVEYARD_SCENE;
		actionAfterLoad = (int)GameState::INTRO; //What
	}
}
