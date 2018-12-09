#include "PanelHierarchy.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

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
	focus = ImGui::IsWindowFocused();
	if (ImGui::SmallButton("Save Scene"))
	{
		App->scene->SaveScene(*App->scene->root, "savedScene");
	}
	if (ImGui::SmallButton("Load Scene"))
	{
		App->scene->LoadScene("BakerHouse");
	}
	ImGui::End();
}