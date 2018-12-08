#ifndef __FileImporter_h__
#define __FileImporter_h__

#include "Importer.h"
#include <assimp/matrix4x4.h>
struct aiScene;
struct aiNode;
struct aiMesh;

class FileImporter :
	public Importer
{
public:
	FileImporter();
	~FileImporter();

	void ImportAsset(const char * file);

	bool ImportmyFBX(const char * file);

	bool ImportScene(const aiScene & scene);

	void ImportMesh(const aiMesh & mesh, char* data);

	unsigned GetMeshSize(const aiMesh & mesh);

	bool ImportFBX(const char* file, const char* path, std::string& output_file) override;
	unsigned int GetNodeSize(const aiNode& node, const aiScene& scene) const;
	bool SaveScene(const aiScene & scene, std::string & output_file);
	bool SaveData(const char * data, unsigned int size, std::string & output_file);
	bool LoadData(std::string & myfile, char * &data);
	void SaveNode(const aiNode & node, const aiScene & scene, char * &cursor, int node_id, int parent_node_id);
	void SaveMesh(const aiMesh & mesh, char * &cursor);
	void ImportImage(const char * file);
};

#endif __FileImporter_h__

