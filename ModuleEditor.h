#ifndef __ModuleEditor_h__
#define __ModuleEditor_h__

#include "Module.h"
class ModuleEditor :
	public Module
{
public:
	ModuleEditor();
	~ModuleEditor();
	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
};

#endif __ModuleEditor_h__
