#ifndef __PanelScene_h__
#define __PanelScene_h__

#include "Panel.h"
#include "Viewport.h"
class PanelScene :
	public Panel
{
public:
	PanelScene();
	~PanelScene();

	void Draw();

private:
	Viewport viewport;
};

#endif //__PanelScene_h__

