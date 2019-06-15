#include "GameStateLoading.h"
#include "Application.h"

#include "GameLoop.h"

#include "ModuleScene.h"


GameStateLoading::GameStateLoading(GameLoop* GL) : GameState(GL)
{
}


GameStateLoading::~GameStateLoading()
{
}

void GameStateLoading::Update()
{
	App->scene->loadScene = true;
	App->scene->name = gLoop->sceneToLoad;
	App->scene->actionAfterLoad = gLoop->actionAfterLoad;
}
