#include "Application.h"

#include "ModuleScene.h"
#include "ComponentAnimation.h"

#include "GameObject.h"
#include "PanelState.h"
#include "ResourceStateMachine.h"

#include "NodeEditor.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#define DEFAULT_BLEND 300

PanelState::PanelState()
{
}

PanelState::~PanelState()
{

}

void PanelState::DrawSM(ResourceStateMachine * animation, ax::NodeEditor::EditorContext * context)
{
	auto& io = ImGui::GetIO();

	ed::SetCurrentEditor(context);
	ed::Begin("State Machine Editor", ImVec2(0.0, 0.0f));

	//ManageCreate(animation);
	//ShowContextMenus(animation);
	int uniqueId = 1;

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


	//ed::Suspend();
	//ShowCreateNewNodeMenu(animation);
	//ed::Resume();

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

void PanelState::ShowContextMenus(ResourceStateMachine* RM)
{
	ed::Suspend();

	ed::NodeId contextNodeId = 0;
	ed::PinId contextPinId = 0;
	ed::LinkId contextLinkId = 0;
	if (ed::ShowNodeContextMenu(&contextNodeId))
	{
		contextNode = unsigned(contextNodeId.Get() - 1) / 3;
		ImGui::OpenPopup("Node Context Menu");
	}
	else if (ed::ShowPinContextMenu(&contextPinId))
	{
		ImGui::OpenPopup("Pin Context Menu");
	}
	else if (ed::ShowLinkContextMenu(&contextLinkId))
	{
		contextLink = unsigned(contextLinkId.Get()) - RM->GetNodesSize() * 3 - 1;
		ImGui::OpenPopup("Link Context Menu");
	}
	else if (ed::ShowBackgroundContextMenu())
	{
		newNodePosition = ImGui::GetMousePos();
		newNodePin = 0;
		ImGui::OpenPopup("Create New Node");
	}

	ed::Resume();
}

void PanelState::ShowCreateNewNodeMenu(ResourceStateMachine* animation)
{
	if (ImGui::BeginPopup("Create New Node"))
	{
		ImGui::TextUnformatted("Create Node Menu");
		ImGui::Separator();

		if (ImGui::BeginMenu("New animation"))
		{
			for (unsigned i = 0, count = animation->GetClipsSize(); i < count; ++i)
			{
				if (ImGui::MenuItem(animation->GetClipName(i).C_str()))
				{
					unsigned nodIdx = animation->GetNodesSize();
					ed::SetNodePosition(nodIdx * 3 + 1, ed::ScreenToCanvas(newNodePosition));

					if (newNodePin != ed::PinId::Invalid)
					{
						unsigned out_node = unsigned(newNodePin.Get() - 1) / 3;
						animation->AddTransition(animation->GetNodeName(out_node), animation->GetNodeName(nodIdx), HashString(), DEFAULT_BLEND);
					}
				}
			}

			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
}

void PanelState::ManageCreate(ResourceStateMachine* animation)
{
	if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
	{
	auto showLabel = [](const char* label, ImColor color)
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
		auto size = ImGui::CalcTextSize(label);

		auto padding = ImGui::GetStyle().FramePadding;
		auto spacing = ImGui::GetStyle().ItemSpacing;

		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

		auto rectMin = ImGui::GetCursorScreenPos() - padding;
		auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

		auto drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
		ImGui::TextUnformatted(label);
	};

	ed::PinId startPinId = 0, endPinId = 0;
	if (ed::QueryNewLink(&startPinId, &endPinId))
	{
		if (startPinId && endPinId)
		{
			bool startIsInput = unsigned(startPinId.Get() - 1) % 3 == 1;
			bool endIsInput = unsigned(endPinId.Get() - 1) % 3 == 1;
			unsigned startNode = unsigned(startPinId.Get() - 1) / 3;
			unsigned endNode = unsigned(endPinId.Get() - 1) / 3;

			if (endPinId == startPinId)
			{
				ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
			}
			else if (startIsInput == endIsInput)
			{
				showLabel("x Incompatible Pins. Must be In->Out", ImColor(45, 32, 32, 180));
				ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
			}
			else if (startNode == endNode)
			{
				showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
				ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
			}
			else
			{
				showLabel("+ Create Link", ImColor(32, 45, 32, 180));
				if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
				{
					if (startIsInput)
					{
						animation->AddTransition(animation->GetNodeName(endNode), animation->GetNodeName(startNode), HashString(), DEFAULT_BLEND);
					}
					else
					{
						animation->AddTransition(animation->GetNodeName(startNode), animation->GetNodeName(endNode), HashString(), DEFAULT_BLEND);
					}

				}
			}
		}
	}

	if (ed::QueryNewNode(&newNodePin))
	{
		bool pinIsInput = unsigned(newNodePin.Get() - 1) % 3 == 1;

		if (!pinIsInput)
		{
			if (newNodePin != ed::PinId::Invalid)
			{
				showLabel("+ Create Node", ImColor(32, 45, 32, 180));
			}

			if (ed::AcceptNewItem())
			{
				ed::Suspend();
				newNodePosition = ImGui::GetMousePos();
				ImGui::OpenPopup("Create New Node");
				ed::Resume();
			}
		}
	}
	}

ed::EndCreate();
}