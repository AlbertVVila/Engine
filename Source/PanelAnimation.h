#ifndef __PanelAnimation_h__
#define __PanelAnimation_h__

#include "Panel.h"

class PanelAnimation : public Panel
{
public:
	PanelAnimation();
	~PanelAnimation();

	void Draw() override;

public:
	int minFrame;
	bool isCliping = false;

};

#endif // !__PanelAnimation_h__