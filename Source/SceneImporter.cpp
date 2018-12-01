#include "SceneImporter.h"
#include <assert.h>
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/mesh.h"
#include "assimp/types.h"
#include <fstream>
#include "IL/ilut.h"
#include "Application.h"
#include "ModuleTextures.h"

#define ASSET_FOLDER "/Assets"
#define LIBRARY_FOLDER "/Library"

SceneImporter::SceneImporter()
{
}


SceneImporter::~SceneImporter()
{
}

bool SceneImporter::ImportFBX(const char * file, const char * path, std::string & output_file)
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
	RELEASE_ARRAY(data);
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

unsigned int SceneImporter::GetNodeSize(const aiNode& node, const aiScene& scene) const //TODO: compute size+ copy at same time
{
	unsigned int size = sizeof(int)*2 + sizeof(float) * 16 + sizeof(char)*(node.mName.length+1); //ids + transform + name

	size += sizeof(int); //numMeshes
	for (unsigned int i = 0; i < node.mNumMeshes; i++)
	{
		aiMesh *mesh = scene.mMeshes[node.mMeshes[i]];
		unsigned int ranges[2] = { mesh->mNumFaces * 3, mesh->mNumVertices };
		size += sizeof(int); //mesh content size
		size += sizeof(ranges);
		size += mesh->mNumFaces * 3 * (sizeof(int)); //indices
		size += sizeof(float)*mesh->mNumVertices*5; //vertices + texCoords
		aiTextureMapping mapping = aiTextureMapping_UV;
		aiString file;
		scene.mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, 0);
		size += sizeof(char)*(file.length + 1);
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
		aiTextureMapping mapping = aiTextureMapping_UV; //we take filename of material mesh
		aiString file;
		scene.mMaterials[scene.mMeshes[node.mMeshes[i]]->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, 0);
		unsigned int materialBytes = sizeof(char)*(file.length + 1);
		memcpy(cursor, file.C_Str(), materialBytes);
		cursor += materialBytes;
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
	unsigned int ranges[2] = { mesh.mNumFaces * 3, 	mesh.mNumVertices };
	unsigned int rangeBytes = sizeof(ranges);
	memcpy(cursor, ranges, rangeBytes);
	cursor += rangeBytes;

	unsigned int verticesBytes = sizeof(float)*mesh.mNumVertices * 3;
	memcpy(cursor, mesh.mVertices, verticesBytes);
	cursor += verticesBytes;

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		memcpy(cursor, &mesh.mTextureCoords[0][i].x, sizeof(float));
		cursor += sizeof(float);
		memcpy(cursor, &mesh.mTextureCoords[0][i].y, sizeof(float));
		cursor += sizeof(float);
	}

	//unsigned int faceBytes = mesh.mNumFaces*3*(sizeof(int));
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		aiFace *face = &mesh.mFaces[i];
		assert(face->mNumIndices == 3);
		memcpy(cursor, face->mIndices, sizeof(int) * 3);
		cursor += sizeof(int) * 3;
	}

}

void SceneImporter::ImportMat(const char * path)
{
	ILuint imageID;
	ILboolean success;
	ILenum error;

	ilGenImages(1, &imageID); 		// Generate the image ID
	ilBindImage(imageID); 			// Bind the image
	success = ilLoadImage(path);
	if (success)
	{

		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}
		ILuint size;
		ILubyte* data = ilGetData();
		ilSetInteger (IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
		size = ilSaveL(IL_DDS, NULL,0);	// Get the size of the data buffer
		data =new ILubyte[size];// allocate data buffer
		if (ilSaveL(IL_DDS, data, size) > 0)
		{
			// Save to buffer with the ilSaveIL function
			std::ofstream outfile;
			outfile.open("test.dds", std::ios::binary | std::ios::out);
			outfile.write((char*)data, size);
			outfile.close();
		}
		RELEASE_ARRAY(data);
	}
}
