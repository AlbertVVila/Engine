#include "GameStatePlaying.h"

#include "GameLoop.h"

#include "GameObject.h"
#include "ComponentButton.h"
#include "ComponentTransform2D.h"

#include "Math/float2.h"
#include "PlayerMovement.h"

#define MENU_SCENE "MenuScene"

GameStatePlaying::GameStatePlaying(GameLoop* GL) : GameState(GL)
{
}


GameStatePlaying::~GameStatePlaying()
{
}

void GameStatePlaying::Update()
{
	if (gLoop->hudBackToMenuButton->IsPressed())
	{
		gLoop->gameState = (GameState*)gLoop->loadingState;
		gLoop->playerMenuGO->SetActive(false);
		gLoop->hudGO->SetActive(false);
		gLoop->loadingGO->SetActive(true);
		gLoop->sceneToLoad = MENU_SCENE;
	}

	if (gLoop->closePlayerMenuButton->KeyUp())
	{
		gLoop->closePlayerMenuButton->isHovered = false;
		gLoop->playerMenuGO->SetActive(false);
		gLoop->inventoryButton->rectTransform->setPosition(math::float2(-50, gLoop->inventoryButton->rectTransform->getPosition().y));
		gLoop->skillsButton->rectTransform->setPosition(math::float2(-50, gLoop->skillsButton->rectTransform->getPosition().y));
		gLoop->missionsButton->rectTransform->setPosition(math::float2(-50, gLoop->missionsButton->rectTransform->getPosition().y));

	}

	if (gLoop->inventoryButton->IsPressed() || gLoop->skillsButton->IsPressed() || gLoop->missionsButton->IsPressed())
	{
		gLoop->playerMenuGO->SetActive(true);
		gLoop->inventoryButton->rectTransform->setPosition(math::float2(-485, gLoop->inventoryButton->rectTransform->getPosition().y));
		gLoop->skillsButton->rectTransform->setPosition(math::float2(-485, gLoop->skillsButton->rectTransform->getPosition().y));
		gLoop->missionsButton->rectTransform->setPosition(math::float2(-485, gLoop->missionsButton->rectTransform->getPosition().y));
		gLoop->inventoryMenuGO->SetActive(gLoop->inventoryButton->IsPressed());
		gLoop->skillsMenuGO->SetActive(gLoop->skillsButton->IsPressed());
		gLoop->missionsMenuGO->SetActive(gLoop->missionsButton->IsPressed());
	}

	gLoop->playerScript->canInteract = true;
	if (gLoop->inventoryButton->IsHovered() || gLoop->skillsButton->IsHovered() || gLoop->missionsButton->IsHovered() || gLoop->closePlayerMenuButton->IsHovered())
	{
		gLoop->playerScript->canInteract = false;
	}
	else if (gLoop->playerMenuGO->activeSelf && gLoop->HasImageHoveredInChildren(gLoop->playerMenuGO))
	{
		gLoop->playerScript->canInteract = false;
	}

	if (gLoop->playerScript->isPlayerDead)
	{
		gLoop->loseWindow->SetActive(true);
		gLoop->gameState = (GameState*)gLoop->deadState;
	}
	else if (gLoop->winBbox->Intersects(*(gLoop->playerBbox)))
	{
		gLoop->winWindow->SetActive(true);
		gLoop->gameState = (GameState*)gLoop->winState;
	}
}
