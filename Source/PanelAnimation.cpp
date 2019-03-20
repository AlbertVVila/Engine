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
		if (App->scene->selected != nullptr && App->scene->selected->isBoneRoot)
		{
			Animation* anim = ((ComponentAnimation*)(App->scene->selected->GetComponent(ComponentType::Animation)))->anim;

		}
		ImGui::End();
		return;
	}

	ImGui::End();
}
