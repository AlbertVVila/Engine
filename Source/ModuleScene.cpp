#include "ModuleScene.h"
#include "Application.h"
#include "GameObject.h"
#include "Component.h"
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


update_status ModuleScene::Update()
{
	root->Update();
	return UPDATE_CONTINUE;
}

void ModuleScene::Draw()
{
		root->Draw();
}

void ModuleScene::DrawHierarchy()
{
	root->DrawHierarchy(selected);
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

GameObject * ModuleScene::DuplicateGameObject(GameObject* gameobject) //TODO: move to Gameobject
{
	/*GameObject * duplicate = new GameObject(gameobject->name.c_str());
	duplicate->filepath = gameobject->filepath;*/

	//for (const auto& component: gameobject->components)
	//{
	//	Component *copy = component->Copy();
	//	duplicate->AddComponent(copy);
	//}
	//for (const auto& child: gameobject->children)
	//{
	//	GameObject* copy = DuplicateGameObject(child);
	//	duplicate->children.push_back(copy);
	//}
	//duplicate->parent = gameobject->parent;
	//duplicate->parent->children.push_back(duplicate);

	GameObject * copy = new GameObject("Test");
	if (gameobject->parent != nullptr)
	{
		copy->parent = gameobject->parent;
		copy->parent->children.push_back(copy);
	}
	return copy;
}