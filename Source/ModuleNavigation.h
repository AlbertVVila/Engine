#ifndef __MODULENAVIGATION_H__
#define __MODULENAVIGATION_H__

#include "Module.h"
#include <vector>
//<>
class GameObject;
class ModuleNavigation :
	public Module
{
public:
	ModuleNavigation();
	~ModuleNavigation();

	//variables
	std::vector<GameObject*> navigationMeshes;
	std::vector<GameObject*> agents;
	std::vector<GameObject*> obstacles;

};

#endif