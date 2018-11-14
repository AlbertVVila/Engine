#include "PanelProperties.h"
#include "ModuleSceneLoader.h"
#include "Application.h"

PanelProperties::PanelProperties()
{
}


PanelProperties::~PanelProperties()
{
}

void PanelProperties::Draw()
{
	if (!ImGui::Begin("Properties", &enabled))
	{
		ImGui::End();
		return;
	}
	App->sceneLoader->DrawModelProperties();
	ImGui::End();
}
