#include "Application.h"

#include "ModuleScene.h"
#include "ModuleEditor.h"

#include "GameObject.h"
#include "Component.h"

#include "PanelInspector.h"
#include "PanelHierarchy.h"
#include "imgui.h"

#define REPEATEDPOPUP "Repeated"
PanelInspector::PanelInspector()
{
}


PanelInspector::~PanelInspector()
{
}

void PanelInspector::Draw() //TODO: Add Light component properties in inspector
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
		const char* components[] = { "Transform", "Mesh", "Material", "Camera", "Light"};

		if (ImGui::Button("Add Component", ImVec2(ImGui::GetWindowWidth(), 25)))
			ImGui::OpenPopup("component_popup");
		ImGui::SameLine();
		if (ImGui::BeginPopup("component_popup"))
		{
			ImGui::Text("Components");
			ImGui::Separator();
			for (int i = 0; i < IM_ARRAYSIZE(components); i++)
				if (ImGui::Selectable(components[i]))
				{
					ComponentType type = (ComponentType)i;
					if (App->scene->selected->GetComponent(type) != nullptr && 
						(type == ComponentType::Renderer || type == ComponentType::Transform))
					{
						openPopup = true;
					}
					else
					{
						App->scene->selected->CreateComponent(type);
					}
				}
			ImGui::EndPopup();
		}
	}
	DrawRepeatedPopup();
	ImGui::End();
}

void PanelInspector::Focus(GameObject *gameobject)
{
	focus = true;
}


void PanelInspector::DrawRepeatedPopup()
{
	if (openPopup && !ImGui::IsPopupOpen(REPEATEDPOPUP))
	{
		ImGui::OpenPopup(REPEATEDPOPUP);
		openPopup = false;
	}
	if (ImGui::BeginPopupModal(REPEATEDPOPUP, NULL))
	{
		ImGui::Text("This GameObject cannot have this component twice! \n\n");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}
}