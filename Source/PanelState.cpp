#include "PanelState.h"



PanelState::PanelState()
{
}

PanelState::~PanelState()
{
}

void PanelState::Draw()
{
	if (!ImGui::Begin("State Machine", &enabled))
	{
		ImGui::End();
		return;
	}

	ImGui::End();
}