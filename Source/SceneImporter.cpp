#include "SceneImporter.h"
#include <assert.h>
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/mesh.h"
#include "assimp/types.h"
#include <fstream>

#define ASSET_FOLDER "/Assets"
#define LIBRARY_FOLDER "/Library"

SceneImporter::SceneImporter()
{
}


SceneImporter::~SceneImporter()
{
}

bool SceneImporter::Import(const char * file, const char * path, std::string & output_file)
{
	assert(path != nullptr);
	const aiScene* scene = aiImportFile(path, aiProcess_Triangulate);
	if (scene != nullptr)
	{
		return SaveScene(*scene, output_file);
	}
	return false;
}

bool SceneImporter::SaveScene(const aiScene & scene, std::string & output_file)
{
	unsigned int size = GetNodeSize(*scene.mRootNode, scene);
	char *data = new char[size];
	char *cursor = data;
	SaveNode(*scene.mRootNode, scene, cursor, 0, -1);
	aiReleaseImport(&scene);
	//Save to file
	return SaveData(data, size, output_file);
}

bool SceneImporter::SaveData(const char *data, unsigned int size, std::string & output_file)
{
	std::ofstream outfile;
	outfile.open(output_file, std::ios::binary | std::ios::out);
	outfile.write(data, size);
	delete[] data;
	outfile.close();
	return true;
}

bool SceneImporter::LoadData(std::string & myfile, char * &data)
{
	std::ifstream file(myfile, std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	data = new char[size];
	if (file.read(data, size))
	{
		return true;
	}
	return false;
}

unsigned int SceneImporter::GetNodeSize(const aiNode& node, const aiScene& scene) const
{
	unsigned int size = sizeof(int)*2 + sizeof(float) * 16 + sizeof(char)*(node.mName.length+1); //ids + transform + name

	size += sizeof(int); //numMeshes
	for (unsigned int i = 0; i < node.mNumMeshes; i++)
	{
		aiMesh *mesh = scene.mMeshes[node.mMeshes[i]];
		/*unsigned int ranges[2] = { mesh->mNumFaces * 3, mesh->mNumVertices };*/
		size += sizeof(int); //num vertices
		size += mesh->mNumFaces * 3 * (sizeof(int)); //indices
		size += sizeof(float)*mesh->mNumVertices*3; //vertices
	}

	size += sizeof(int); //numChildren
	for (unsigned int i = 0; i < node.mNumChildren; i++)
	{
		size += GetNodeSize(*node.mChildren[i], scene);
	}
	return size;
}

void SceneImporter::SaveNode(const aiNode& node, const aiScene & scene, char * &cursor, int node_id, int parent_node_id)
{
	unsigned int id_bytes = sizeof(int);
	memcpy(cursor, &node_id, id_bytes);
	cursor += id_bytes;

	memcpy(cursor, &parent_node_id, id_bytes);
	cursor += id_bytes;

	float* transform = (float*)&node.mTransformation;
	unsigned int transformBytes = sizeof(float) * 16;
	memcpy(cursor, transform, transformBytes);
	cursor += transformBytes;
	
	unsigned int nameBytes = sizeof(char) * (node.mName.length+1);
	memcpy(cursor, node.mName.C_Str(), nameBytes);
	cursor += nameBytes;

	memcpy(cursor, &node.mNumMeshes, sizeof(int));
	cursor += sizeof(int);

	for (unsigned int i = 0; i < node.mNumMeshes; i++)
	{
		SaveMesh(*scene.mMeshes[node.mMeshes[i]], cursor);
	}

	memcpy(cursor, &node.mNumChildren, sizeof(int));
	cursor += sizeof(int);

	unsigned int new_id = node_id; //parent
	for (unsigned int i = 0; i < node.mNumChildren; i++)
	{
		SaveNode(*node.mChildren[i], scene, cursor, ++new_id, node_id);
	}
}

void SceneImporter::SaveMesh(const aiMesh& mesh, char * &cursor)
{
	unsigned int nverticesBytes = sizeof(int);
	memcpy(cursor, &mesh.mNumVertices, nverticesBytes);
	cursor += nverticesBytes;

	//unsigned int faceBytes = mesh.mNumFaces*3*(sizeof(int));
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		aiFace *face = &mesh.mFaces[i];
		assert(face->mNumIndices == 3);
		memcpy(cursor, face->mIndices, sizeof(int) * 3);
		cursor += sizeof(int) * 3;
	}

	unsigned int verticesBytes = sizeof(float)*mesh.mNumVertices*3;
	memcpy(cursor, mesh.mVertices, verticesBytes);
	cursor += verticesBytes;
}
