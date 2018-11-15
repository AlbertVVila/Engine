#include "PanelInspector.h"
#include "ModuleScene.h"
#include "Application.h"
#include "GameObject.h"

PanelInspector::PanelInspector()
{
}


PanelInspector::~PanelInspector()
{
}

void PanelInspector::Draw()
{
	ImGui::Begin("Inspector", &enabled);
	//if (!ImGui::Begin("Inspector", &enabled))
	//{
	//	ImGui::End();
	//	return;
	//}
	if (focus)
	{
		focus = false;
		ImGui::SetWindowFocus();
	}
	if (current_gameobject != nullptr)
	{
		current_gameobject->DrawProperties();
	}
	ImGui::End();
}

void PanelInspector::Show(GameObject *gameobject)
{
	focus = true;
	current_gameobject = gameobject;
}
