#ifndef __ModuleScene_h__
#define __ModuleScene_h__

#include "Module.h"
#include "assimp/matrix4x4.h"

class GameObject;
class ComponentCamera;
class Texture;

class ModuleScene :
	public Module
{
public:
	ModuleScene();
	~ModuleScene();

	bool Start() override;

	update_status Update() override;
	void Draw();
	void DrawHierarchy();

	GameObject * CreateGameObject(const aiMatrix4x4& transform, const char* filepath, const char* name, GameObject* parent);
	GameObject * CreateGameObject(const char * name, GameObject* parent);

public:
	GameObject* root = nullptr;
	GameObject* selected = nullptr;
	ComponentCamera* maincamera = nullptr;
	Texture* camera_notfound_texture = nullptr;
};

#endif __ModuleScene_h__