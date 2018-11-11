#ifndef __PanelScene_h__
#define __PanelScene_h__

#include "Panel.h"
class PanelScene :
	public Panel
{
public:
	PanelScene();
	~PanelScene();

	void Draw();
	bool IsFocused() const;

private:
	bool focus = false;

};

#endif //__PanelScene_h__

