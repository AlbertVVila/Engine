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

	void OpenResourceEditor(Resource* resource);

public:

	Resource* auxResource;
	Resource* previous;

	bool openEditor = false;
};

#endif __PanelResourceManager_h__