#include "PanelProperties.h"
#include "ModuleFiles.h"
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
	App->model->DrawModelProperties();
	ImGui::End();
}
