#include "GameStateControls.h"

#include "GameLoop.h"

#include "GameObject.h"
#include "ComponentButton.h"

GameStateControls::GameStateControls(GameLoop* GL) : GameState(GL)
{
}


GameStateControls::~GameStateControls()
{
}

void GameStateControls::Update()
{
	if (gLoop->backControlsButton->IsPressed())
	{
		gLoop->backControlsButton->isHovered = false;
		gLoop->backControlsButton->isKeyDown = false;
		gLoop->controls->SetActive(false);
		gLoop->EnableMenuButtons(true);
		gLoop->gameState = (GameState*)gLoop->menuState;
	}
}
