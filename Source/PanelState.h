#ifndef __PANELSTATE_H_
#define __PANELSTATE_H_

#include "Panel.h"

#include "NodeEditor.h"

class PanelState :
	public Panel
{
public:
	PanelState();
	~PanelState();

	void Draw() override;

};

#endif // __PANELSTATE_H_