#ifndef __PanelInspector_h__
#define __PanelInspector_h__

#include "Panel.h"
class GameObject;

class PanelInspector :
	public Panel
{
public:
	PanelInspector();
	~PanelInspector();

	void Draw() override;
	void Focus(GameObject *gameobject);

private:
	bool focus = false;
};

#endif //__PanelInspector_h__

