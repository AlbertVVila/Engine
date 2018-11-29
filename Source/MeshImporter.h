#ifndef __MeshImporter_h__
#define __MeshImporter_h__

#include "Importer.h"
class aiScene;
class aiNode;
class aiMesh;

class MeshImporter :
	public Importer
{
public:
	MeshImporter();
	~MeshImporter();

	bool Import(const char* file, const char* path, std::string& output_file) override;
	void SaveScene(const aiScene * scene);
	void SaveNode(const aiNode & node, const aiScene & scene, const aiMatrix4x4 & parentTransform);
	void SaveMesh(const aiMesh & mesh);
};

#endif __MeshImporter_h__

