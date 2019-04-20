#ifndef __PANELSTATE_H_
#define __PANELSTATE_H_

#include "Panel.h"

#include "imgui.h"

#include "NodeEditor.h"

namespace ed = ax::NodeEditor;

class ResourceStateMachine;
class ComponentAnimation;

class PanelState : public Panel
{
public:
	PanelState();
	~PanelState();

	void DrawSM(ResourceStateMachine* stateMachine, ax::NodeEditor::EditorContext* context);
	void DrawNodes(ResourceStateMachine* stateMachine);
	void ShowContextMenus(ResourceStateMachine* stateMachine);
	void ShowCreateNewNodeMenu(ResourceStateMachine* stateMachine);
	void ManageCreate(ResourceStateMachine* stateMachine);
	void Draw() override;

	bool focus = false;
private:
	unsigned contextNode = 0u;
	unsigned contextLink = 0u;

	ImVec2 newNodePosition;
	ed::PinId newNodePin = 0;
};

#endif // __PANELSTATE_H_