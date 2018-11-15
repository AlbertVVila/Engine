#ifndef __ModuleSceneLoader_h__
#define __ModuleSceneLoader_h__

#include "Module.h"
#include "Model.h"
#include <list>

struct aiScene;
struct Texture;
class GameObject;

class ModuleSceneLoader :
	public Module
{
public:
	ModuleSceneLoader();
	~ModuleSceneLoader();

	bool Init();
	bool Start();
	void LoadFile(const char *path);
	void LoadScene(const aiScene* scene);
	GameObject* ProcessNode(const aiNode * node, const aiScene * scene, const aiMatrix4x4 &parentTransform);




	void DrawModels();
	void DrawModelProperties();
	void DrawGUI();
	void ApplyTexture(Texture texture);
	bool CleanUp();

private:
	void DeleteModels();

public:
	std::list<Model> models;
	bool checkers = false;
	Texture checkersTexture = Texture(0,0,0);
	const char *filepath = nullptr;
};

#endif //__ModuleSceneLoader_h__
