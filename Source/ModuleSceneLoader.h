#ifndef __ModuleSceneLoader_h__
#define __ModuleSceneLoader_h__

#include "Module.h"
#include <list>
#include "assimp/matrix4x4.h"
#include "Math/float3.h"
#include "Math/float4.h"
#include "Math/Quat.h"

struct aiNode;
struct aiScene;
class GameObject;
class MeshImporter;
class MaterialImporter;

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

	void CreateSphere(const char * name, const float3 & pos, const Quat & rot, float size, unsigned int slices, unsigned int stacks, const float4 & color);

	//void ApplyTexture(Texture* texture);
	bool CleanUp();

private:

public:
	//bool checkers = false;
	//Texture checkersTexture = Texture(0,0,0);
	const char *filepath = nullptr; //TODO: refactor class

private:
	MaterialImporter * matImporter = nullptr;
	MeshImporter * meshImporter = nullptr;
};

#endif //__ModuleSceneLoader_h__
