#ifndef __ModuleScene_h__
#define __ModuleScene_h__

#include "Module.h"

class GameObject;

class ModuleScene :
	public Module
{
public:
	ModuleScene();
	~ModuleScene();

	GameObject * CreateGameObject();
};

#endif __ModuleScene_h__