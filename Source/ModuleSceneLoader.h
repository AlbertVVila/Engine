#ifndef __ModuleSceneLoader_h__
#define __ModuleSceneLoader_h__

#include "Module.h"
#include <list>
#include "assimp/matrix4x4.h"

struct aiNode;
struct aiScene;
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
	GameObject* ProcessNode(const aiNode * node, const aiScene * scene, const aiMatrix4x4 &parentTransform, GameObject* parent);

	//void ApplyTexture(Texture* texture);
	bool CleanUp();

private:

public:
	//bool checkers = false;
	//Texture checkersTexture = Texture(0,0,0);
	const char *filepath = nullptr;
};

#endif //__ModuleSceneLoader_h__
