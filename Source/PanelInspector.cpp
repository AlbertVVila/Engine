#include "PanelInspector.h"
#include "ModuleScene.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleEditor.h"
#include "PanelHierarchy.h"

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
	if (App->scene->selected != nullptr)
	{
		App->scene->selected->DrawProperties();
		const char* components[] = { "Transform", "Mesh", "Material"};

		if (ImGui::Button("Add Component", ImVec2(ImGui::GetWindowWidth(), 25)))
			ImGui::OpenPopup("component_popup");
		ImGui::SameLine();
		if (ImGui::BeginPopup("component_popup"))
		{
			ImGui::Text("Components");
			ImGui::Separator();
			for (int i = 0; i < IM_ARRAYSIZE(components); i++)
				if (ImGui::Selectable(components[i]))
					App->scene->selected->CreateComponent((ComponentType)i);
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void PanelInspector::Focus(GameObject *gameobject)
{
	focus = true;
}
