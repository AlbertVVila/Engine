#include "LoopStateDead.h"

#include "GameLoop.h"

#include "GameObject.h"
#include "ComponentButton.h"

#define GRAVEYARD_SCENE "Level0-TheGraveyard"

LoopStateDead::LoopStateDead(GameLoop* GL) : LoopState(GL)
{
}


LoopStateDead::~LoopStateDead()
{
}

void LoopStateDead::Update()
{
	if (gLoop->toTheAltarButton->IsPressed())
	{
		gLoop->loseWindow->SetActive(false);
		gLoop->loadingGO->SetActive(true);
		gLoop->playerMenuGO->SetActive(false);
		gLoop->currentLoopState = (LoopState*)gLoop->loadingState;
		gLoop->sceneToLoad = GRAVEYARD_SCENE;
		//actionAfterLoad = (int)GameState::INTRO; //What
	}
}
