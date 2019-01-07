#ifndef __PanelCamera_h__
#define __PanelCamera_h__

#include "Panel.h"
#include "Viewport.h"

class PanelCamera :
	public Panel
{
public:
	PanelCamera();
	~PanelCamera();

	void Draw();

private:
	Viewport viewport;
};

#endif __PanelCamera_h__
