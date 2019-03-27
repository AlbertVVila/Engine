#include "ModuleNavigation.h"
#include "GameObject.h"

#include "imgui.h"

ModuleNavigation::ModuleNavigation()
{
}


ModuleNavigation::~ModuleNavigation()
{
}

void ModuleNavigation::DrawGUI()
{
	if (ImGui::CollapsingHeader("Agents"))
	{

	}
	if (ImGui::CollapsingHeader("Areas"))
	{

	}
	if (ImGui::CollapsingHeader("Bake"))
	{
		ImGui::DragFloat("Agent max radius", &maxRadius, sliderIncreaseSpeed, minSliderValue, maxSliderValue);
		ImGui::DragFloat("Agent max height", &maxHeight, sliderIncreaseSpeed, minSliderValue, maxSliderValue);
		ImGui::DragFloat("Max slope scaling", &maxSlopeScaling, sliderIncreaseSpeed, minSliderValue, maxSlopeValue);
		ImGui::DragFloat("Max step height", &maxStepHeightScaling, sliderIncreaseSpeed, minSliderValue, maxSlopeValue);

	}
	if (ImGui::CollapsingHeader("Object"))
	{

	}
}

void ModuleNavigation::navigableObjectToggled(GameObject* obj, const bool newState)
{
	if (newState) navigationMeshes.push_back(obj);
	else removeNavMesh(obj->UUID);
}

void ModuleNavigation::removeNavMesh(unsigned ID)
{
	for (int i = 0; i < navigationMeshes.size(); ++i)
	{
		if (navigationMeshes[i]->UUID == ID)
		{
			navigationMeshes.erase(navigationMeshes.begin() + i);
			return;
		}
	}
}