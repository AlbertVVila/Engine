#include "GameStateCredits.h"

#include "GameLoop.h"

#include "GameObject.h"
#include "ComponentButton.h"

#include "CreditsScript.h"

GameStateCredits::GameStateCredits(GameLoop* GL) : GameState(GL)
{
}


GameStateCredits::~GameStateCredits()
{
}

void GameStateCredits::Enter()
{
	gLoop->menu->SetActive(false);
}

void GameStateCredits::Update()
{
	if (!(gLoop->runningCredits))
	{
		gLoop->creditsGO->SetActive(true);
		gLoop->creditsScript->Enable(true);
		gLoop->creditsScript->Start();
		gLoop->runningCredits = true;
	}
	else if (gLoop->creditsScript->creditsDone)
	{
		gLoop->creditsGO->SetActive(false);
		gLoop->menu->SetActive(true);
		gLoop->runningCredits = false;
		gLoop->creditsScript->ResetScript();
		gLoop->gameState = (GameState*)gLoop->menuState;
	}

	if (gLoop->backCreditsButton->IsPressed())
	{
		gLoop->creditsScript->creditsDone = true;
	}
}
