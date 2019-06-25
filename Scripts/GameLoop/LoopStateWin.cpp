#include "LoopStateWin.h"
#include "Application.h"

#include "ModuleTime.h"
#include "ModuleScene.h"

#include "GameLoop.h"

#include "GameObject.h"
#include "InventoryScript.h"
#include "EquipPopupController.h"
#include "SkillTreeController.h"
#include "ExperienceController.h"
#include "PlayerMovement.h"

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

		//Save game stats
		gLoop->inventoryScript->SaveInventory();
		gLoop->skillTreeScript->SaveSkillTree();
		gLoop->experienceScript->SaveExperience();
		gLoop->equipPopUpScript->SavePopUp();
		gLoop->playerScript->SavePlayerStats();

		gLoop->currentLoopState = (LoopState*)gLoop->loadingState;
		gLoop->playerMenuGO->SetActive(false);
		gLoop->sceneToLoad = MENU_SCENE;
		gLoop->App->scene->actionAfterLoad = true;
		gLoop->App->scene->stateAfterLoad = "Credits";
		gLoop->stateAfterLoad = (LoopState*)gLoop->creditsState;
	}
}
