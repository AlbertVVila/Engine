#include "PanelProperties.h"



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
	ImGui::End();
}
