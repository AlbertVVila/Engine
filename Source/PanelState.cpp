#include "Application.h"

#include "ModuleScene.h"
#include "ComponentAnimation.h"

#include "GameObject.h"
#include "PanelState.h"

#include "NodeEditor.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"



PanelState::PanelState()
{
}

PanelState::~PanelState()
{

}

void PanelState::DrawSM(ResourceStateMachine * animation, ax::NodeEditor::EditorContext * context)
{

	ed::SetCurrentEditor(context);
	ed::Begin("State Machine Editor", ImVec2(0.0, 0.0f));

	int uniqueId = 1;

	// Start drawing nodes.

	ed::BeginNode(uniqueId++);

	ImGui::Text("Node A");

	ed::BeginPin(uniqueId++, ed::PinKind::Input);

	ImGui::Text("-> In");

	ed::EndPin();

	ImGui::SameLine();

	ed::BeginPin(uniqueId++, ed::PinKind::Output);

	ImGui::Text("Out ->");

	ed::EndPin();

	ed::EndNode();

	ed::End();
	ed::SetCurrentEditor(nullptr);

}

void PanelState::Draw()
{
	if (!ImGui::Begin("State Machine", &enabled))
	{
		ImGui::End();
		return;
	}

	if (App->scene->selected != nullptr && (ComponentAnimation*)(App->scene->selected->GetComponent(ComponentType::Animation)))
	{
		ComponentAnimation* compAnim = (ComponentAnimation*)(App->scene->selected->GetComponent(ComponentType::Animation));

		DrawSM(compAnim->stateMachine, compAnim->GetEditorContext());
	}

	ImGui::End();
}