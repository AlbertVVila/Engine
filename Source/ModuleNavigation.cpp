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
		ImGui::InputText("New Character", newCharacter, 64);
		ImGui::DragFloat("Character Radius", &characterMaxRadius, sliderIncreaseSpeed, minSliderValue, maxSliderValue);
		ImGui::DragFloat("Height", &characterMaxHeight, sliderIncreaseSpeed, minSliderValue, maxSliderValue);
		ImGui::DragFloat("Max slope", &characterMaxSlopeScaling, sliderIncreaseSpeed, minSliderValue, maxSlopeValue);
		ImGui::DragFloat("Max step height", &characterMaxStepHeightScaling, sliderIncreaseSpeed, minSliderValue, maxSlopeValue);
		ImGui::Button("Add Character", ImVec2(ImGui::GetWindowWidth(), 25));
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

		if (ImGui::Button("Generate navigability"))
		{
			/*if ()
			{

			}*/
			generator->GenerateNavigability();
		}
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