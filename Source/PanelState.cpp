#include "PanelState.h"



PanelState::PanelState()
{
}

PanelState::~PanelState()
{
}

void PanelState::Draw()
{
	ImGui::Begin("StateMachine", &enabled, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);


	ImGui::End();
}