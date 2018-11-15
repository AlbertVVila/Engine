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
	for (auto &gameobject : gameobjects)
	{
		gameobject->Draw();
	}
}

void ModuleScene::DrawHierarchy()
{
	int selection_mask = (1 << obj_clicked);
	int i = 0;
	for (auto &gameobject : gameobjects)
	{
		//if (gameobject->parent != nullptr) continue;
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);

		ImGui::PushID(&gameobject);
		bool obj_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, gameobject->name.c_str());
		if (ImGui::IsItemClicked())
		{
			obj_clicked = i;
			App->editor->ShowInspector(gameobject);
		}
		if (obj_open)
		{
			
			ImGui::TreePop();
		}
		ImGui::PopID();
			//for (auto &child : gameobject->children)
			//{
			//	if (ImGui::TreeNode(child->name.c_str()))
			//	{
			//		ImGui::TreePop();
			//	}
			//}
		++i;
	}
	if (obj_clicked != -1)
	{
		selection_mask = (1 << obj_clicked);
	}
}

GameObject * ModuleScene::CreateGameObject(const aiMatrix4x4 & transform, const char * filepath, const char * name)
{
	gameobjects.push_back(new GameObject(transform, filepath, name));
	return gameobjects.back();
}
