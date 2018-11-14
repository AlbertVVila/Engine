#ifndef __ModuleScene_h__
#define __ModuleScene_h__

#include "Module.h"
#include "assimp/matrix4x4.h"
#include <vector>

class GameObject;

class ModuleScene :
	public Module
{
public:
	ModuleScene();
	~ModuleScene();
	void Draw();

	GameObject * CreateGameObject(const aiMatrix4x4& transform);
public:
	std::vector<GameObject *> gameobjects;
};

#endif __ModuleScene_h__