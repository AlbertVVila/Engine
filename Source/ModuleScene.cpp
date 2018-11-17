#include "ModuleScene.h"
#include "Application.h"
#include "GameObject.h"
#include "Imgui/imgui.h"
#include "ModuleEditor.h"
ModuleScene::ModuleScene()
{
}


ModuleScene::~ModuleScene()
{
}

void ModuleScene::Draw()
{
		root->Draw();
}

void ModuleScene::DrawHierarchy()
{
	root->DrawHierarchy(obj_clicked,0);
}

GameObject * ModuleScene::CreateGameObject(const aiMatrix4x4 & transform, const char * filepath, const char * name)
{
	GameObject * gameobject = new GameObject(transform, filepath, name);
	if (root == nullptr)
	{
		root = gameobject;
	}
	return gameobject;
}
