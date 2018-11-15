#include "PanelInspector.h"
#include "ModuleSceneLoader.h"
#include "Application.h"

PanelInspector::PanelInspector()
{
}


PanelInspector::~PanelInspector()
{
}

void PanelInspector::Draw()
{
	if (!ImGui::Begin("Inspector", &enabled))
	{
		ImGui::End();
		return;
	}
	App->sceneLoader->DrawModelProperties();
	ImGui::End();
}
