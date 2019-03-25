#include "PanelState.h"

#include "imgui.h"
#include "NodeEditor.h"

//#define IMGUI_DEFINE_MATH_OPERATORS
//#include "imgui_internal.h"

PanelState::PanelState()
{
}

PanelState::~PanelState()
{
	/*if (context)
	{
		ed::DestroyEditor(context);
		context = nullptr;
	}*/
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