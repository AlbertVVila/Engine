#include "PanelNavigation.h"
#include "Application.h"
#include "ModuleNavigation.h"
#include "imgui.h"


PanelNavigation::PanelNavigation()
{
}


PanelNavigation::~PanelNavigation()
{
}


void PanelNavigation::Draw()
{
	if (!ImGui::Begin("Navigation", &enabled))
	{
		ImGui::End();
		return;
	}
	App->navigation->DrawGUI();
	ImGui::End();
}