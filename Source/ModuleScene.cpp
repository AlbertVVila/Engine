#include "ModuleScene.h"
#include "GameObject.h"
#include "Imgui/imgui.h"
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

void ModuleScene::DrawHierarchy()
{
	for (auto &gameobject : gameobjects)
	{
		if (gameobject->parent != nullptr) continue;
		if (ImGui::TreeNode("Parent"))
		{
			for (auto &child : gameobject->children)
			{
				if (ImGui::TreeNode(child->name.c_str()))
				{
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
}

GameObject * ModuleScene::CreateGameObject(const aiMatrix4x4 & transform, const char * filepath, const char * name)
{
	gameobjects.push_back(new GameObject(transform, filepath, name));
	return gameobjects.back();
}
