#include "PanelNavigation.h"
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
	if (ImGui::CollapsingHeader("Agents"))
	{
		
	}
	if (ImGui::CollapsingHeader("Areas"))
	{
		
	}
	if (ImGui::CollapsingHeader("Bake"))
	{
		
	}
	if (ImGui::CollapsingHeader("Object"))
	{
		
	}
	ImGui::End();
}