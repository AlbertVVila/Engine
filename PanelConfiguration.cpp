#include "PanelConfiguration.h"



PanelConfiguration::PanelConfiguration()
{
}


PanelConfiguration::~PanelConfiguration()
{
}

void PanelConfiguration::Draw()
{
	if (!ImGui::Begin("Configuration", &enabled))
	{
		ImGui::End();
		return;
	}
	if (ImGui::CollapsingHeader("Window"))
	{
		
	}
	if (ImGui::CollapsingHeader("Camera"))
	{

	}
	if (ImGui::CollapsingHeader("Render"))
	{

	}
	if (ImGui::CollapsingHeader("Editor"))
	{

	}
	if (ImGui::CollapsingHeader("Textures"))
	{

	}
	ImGui::End();
}

