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
	if (!ImGui::Begin("Inspector", &enabled))
	{
		ImGui::End();
		return;
	}
	if (focus)
	{
		focus = false;
		ImGui::SetWindowFocus();
	}
	if (current_gameobject != nullptr)
	{
		current_gameobject->DrawProperties();
		const char* components[] = { "Transform", "Mesh", "Material"};

		// Simple selection popup
		// (If you want to show the current selection inside the Button itself, you may want to build a string using the "###" operator to preserve a constant ID with a variable label)
		if (ImGui::Button("Add Component", ImVec2(ImGui::GetWindowWidth(), 25)))
			ImGui::OpenPopup("component_popup");
		ImGui::SameLine();
		if (ImGui::BeginPopup("component_popup"))
		{
			ImGui::Text("Components");
			ImGui::Separator();
			for (int i = 0; i < IM_ARRAYSIZE(components); i++)
				if (ImGui::Selectable(components[i]))
					current_gameobject->CreateComponent((ComponentType)i);
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void PanelInspector::Show(GameObject *gameobject)
{
	focus = true;
	current_gameobject = gameobject;
}
