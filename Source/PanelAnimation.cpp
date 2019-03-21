#include "Application.h"

#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentAnimation.h"
#include "PanelAnimation.h"
#include "imgui.h"
#include "Globals.h"

PanelAnimation::PanelAnimation()
{
}


PanelAnimation::~PanelAnimation()
{
}

void PanelAnimation::Draw()
{
	if (!ImGui::Begin("Animation", &enabled))
	{
		ImGui::End();
		return;
	}
	if (App->scene->selected != nullptr && App->scene->selected->isBoneRoot)
	{
		Animation* anim = ((ComponentAnimation*)(App->scene->selected->GetComponent(ComponentType::Animation)))->anim;

		ImGui::Text("GAMEOBJECT");

		ImGui::Separator();
		ImGui::Text(App->scene->selected->name.c_str());

	}

	ImGui::End();
}
