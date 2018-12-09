#ifndef __FileImporter_h__
#define __FileImporter_h__

#include <assimp/matrix4x4.h>
#include <map>
#include "GameObject.h"

struct aiScene;
struct aiNode;
struct aiMesh;


class FileImporter {
public:
	FileImporter();
	~FileImporter();

	void ImportAsset(const char * file);

	GameObject* ProcessNode(const std::map<unsigned,unsigned> &meshmap, const aiNode * node, const aiScene * scene, const aiMatrix4x4 & parentTransform, GameObject * parent);

	bool ImportmyFBX(const char * file);

	bool ImportScene(const aiScene & scene, const char* file);

	void ImportMesh(const aiMesh & mesh, char* data);

	unsigned GetMeshSize(const aiMesh & mesh);
};

#endif __FileImporter_h__

