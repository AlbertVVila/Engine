#ifndef __PANELSTATE_H_
#define __PANELSTATE_H_

#include "Panel.h"

#include "imgui.h"

#include "NodeEditor.h"

namespace ed = ax::NodeEditor;

class ResourceStateMachine;
class ComponentAnimation;

enum class PinKind
{
	Output,
	Input
};

class PanelState : public Panel
{
public:
	PanelState();
	~PanelState();

	void DrawSM(ResourceStateMachine* animation, ax::NodeEditor::EditorContext* context);
	
	void Draw() override;

};

#endif // __PANELSTATE_H_