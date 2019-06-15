#include "GameStateWin.h"
#include "Application.h"

#include "GameLoop.h"

GameStateWin::GameStateWin(GameLoop* GL) : GameState(GL)
{
}


GameStateWin::~GameStateWin()
{
	gLoop->loading += App->time->gameDeltaTime;

	if (gLoop->loading >= 1.0f)
	{
		gLoop->loading = 0.0;
		//ResetPositions();
		gLoop->ChangeGameState(GameState::LOADING);
		gLoop->playerMenuGO->SetActive(false);
		sceneToLoad = MENU_SCENE;
		actionAfterLoad = (int)GameState::CREDITS;
	}
}
