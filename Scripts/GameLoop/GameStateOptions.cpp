#include "GameStateOptions.h"

#include "GameLoop.h"

#include "GameObject.h"
#include "ComponentButton.h"

GameStateOptions::GameStateOptions(GameLoop* GL) : GameState(GL)
{
}


GameStateOptions::~GameStateOptions()
{
}

void GameStateOptions::Update()
{
	gLoop->VolumeManagement();
	gLoop->SoundManagement();
	gLoop->VsyncManagement();
	gLoop->ResolutionManagement();
	if (gLoop->backOptionButton->IsPressed())
	{
		gLoop->backOptionButton->isHovered = false;
		gLoop->backOptionButton->isKeyDown = false;
		gLoop->options->SetActive(false);
		gLoop->EnableMenuButtons(true);
		gLoop->gameState = (GameState*)gLoop->menuState;
	}
}
