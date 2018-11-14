#include "ModuleScene.h"
#include "GameObject.h"

ModuleScene::ModuleScene()
{
}


ModuleScene::~ModuleScene()
{
}

void ModuleScene::Draw()
{
	for (auto &gameobject : gameobjects)
	{
		gameobject->Draw();
	}
}

GameObject * ModuleScene::CreateGameObject(const aiMatrix4x4 & transform)
{
	gameobjects.push_back(new GameObject(transform));
	return gameobjects.back();
}
