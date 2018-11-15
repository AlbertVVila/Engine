#include "PanelHierarchy.h"
#include "Application.h"
#include "ModuleScene.h"


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
