#include "ModuleScene.h"
#include "Application.h"
#include "GameObject.h"
#include "Imgui/imgui.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
ModuleScene::ModuleScene()
{
	root = new GameObject("World");
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
	root->DrawHierarchy(obj_clicked, 0);
}

GameObject * ModuleScene::CreateGameObject(const aiMatrix4x4 & transform, const char * filepath, const char * name, GameObject* parent)
{
	GameObject * gameobject = new GameObject(transform, filepath, name);
	if (parent != nullptr)
	{
		gameobject->parent = parent;
		parent->children.push_back(gameobject);
	}
	return gameobject;
}

GameObject * ModuleScene::CreateGameObject(const char * name, GameObject* parent)
{
	GameObject * gameobject = new GameObject(name);
	if (parent != nullptr)
	{
		gameobject->parent = parent;
		parent->children.push_back(gameobject);
	}
	return gameobject;
}
