#ifndef __FileImporter_h__
#define __FileImporter_h__

#include <map>
#include "Resource.h"
#include "ResourceMesh.h"

struct aiScene;
struct aiNode;
struct aiMesh;
class GameObject;

class FileImporter {
public:
	FileImporter();
	~FileImporter();

	void ImportAsset(const char * file, const char* folder);

	GameObject* ProcessNode(const std::map<unsigned,unsigned> &meshmap, const aiNode * node, const aiScene * scene, GameObject * parent);

	bool ImportFBX(const char * file, const char* folder, ResourceMesh* resource);

	bool ImportScene(const aiScene & scene, const char* file, const char* folder, ResourceMesh* resource);

	void ImportMesh(const aiMesh & mesh, char* data);

	unsigned GetMeshSize(const aiMesh & mesh) const;
};

#endif __FileImporter_h__

