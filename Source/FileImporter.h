#ifndef __FileImporter_h__
#define __FileImporter_h__

#include <assimp/matrix4x4.h>
#include <map>

struct aiScene;
struct aiNode;
struct aiMesh;
class GameObject;

class FileImporter {
public:
	FileImporter();
	~FileImporter();

	void ImportAsset(const char * file, const char* folder);

	GameObject* ProcessNode(const std::map<unsigned,unsigned> &meshmap, const aiNode * node, const aiScene * scene, const aiMatrix4x4 & parentTransform, GameObject * parent);

	bool ImportFBX(const char * file, const char* folder);

	bool ImportScene(const aiScene & scene, const char* file);

	void ImportMesh(const aiMesh & mesh, char* data);

	unsigned GetMeshSize(const aiMesh & mesh) const;
};

#endif __FileImporter_h__

