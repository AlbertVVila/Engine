#include "PanelHierarchy.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

PanelHierarchy::PanelHierarchy()
{
}


PanelHierarchy::~PanelHierarchy()
{
}

void PanelHierarchy::Draw()
{
	if (!ImGui::Begin("Hierarchy", &enabled))
	{
		ImGui::End();
		return;
	}
	App->scene->DrawHierarchy();
	ImGui::End();
}

void PanelHierarchy::SelectGameObject(GameObject *object)
{
	if (current_gameobject != nullptr)
	{
		current_gameobject->DisableBox();
	}
	current_gameobject = object;
}
