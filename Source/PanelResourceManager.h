#ifndef __PanelResourceManager_h__
#define __PanelResourceManager_h__

#include "Panel.h"

class Resource;

class PanelResourceManager : 
	public Panel
{
public:
	PanelResourceManager();
	~PanelResourceManager();

	void Draw() override;
};

#endif __PanelResourceManager_h__