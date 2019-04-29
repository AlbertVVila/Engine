#include "NPCController.h"

#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"

NPCController_API Script* CreateScript()
{
	NPCController* instance = new NPCController;
	return instance;
}

void NPCController::Start()
{
	player = App->scene->FindGameObjectByName(App->scene->root, playerName.c_str());

	myBbox = &App->scene->FindGameObjectByName(gameObject, myBboxName.c_str())->bbox;
	playerBbox = &App->scene->FindGameObjectByName(player, playerBboxName.c_str())->bbox;
}

void NPCController::Update()
{

}
