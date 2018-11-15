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
	void Show(GameObject *gameobject);

private:
	GameObject * current_gameobject = nullptr;
	bool focus = false;
};

#endif //__PanelInspector_h__

