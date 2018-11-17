#ifndef __ModuleScene_h__
#define __ModuleScene_h__

#include "Module.h"
#include "assimp/matrix4x4.h"

class GameObject;

class ModuleScene :
	public Module
{
public:
	ModuleScene();
	~ModuleScene();
	void Draw();
	void DrawHierarchy();

	GameObject * CreateGameObject(const aiMatrix4x4& transform, const char* filepath, const char* name);
public:
	GameObject* root = nullptr;
	int obj_clicked = -1;
};

#endif __ModuleScene_h__