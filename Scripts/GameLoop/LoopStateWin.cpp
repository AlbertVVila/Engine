#include "LoopStateWin.h"
#include "Application.h"

#include "ModuleTime.h"

#include "GameLoop.h"

#include "GameObject.h"

#define MENU_SCENE "MenuScene"

LoopStateWin::LoopStateWin(GameLoop* GL) : LoopState(GL)
{
}


LoopStateWin::~LoopStateWin()
{
}

void LoopStateWin::Update()
{
	gLoop->loading += gLoop->App->time->gameDeltaTime;

	if (gLoop->loading >= 1.0f)
	{
		gLoop->loading = 0.0;
		//ResetPositions();
		gLoop->currentLoopState = (LoopState*)gLoop->loadingState;
		gLoop->playerMenuGO->SetActive(false);
		gLoop->sceneToLoad = MENU_SCENE;
		//gLoop->actionAfterLoad = (int)GameState::CREDITS;
	}
}
