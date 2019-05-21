#ifndef __PanelAbout_h__
#define __PanelAbout_h__

#include "Panel.h"
class PanelAbout : public Panel
{
public:
	PanelAbout();
	~PanelAbout();

	void Draw();

public:
	const char * freeTypeVersion;
};

#endif //__PanelAbout_h__