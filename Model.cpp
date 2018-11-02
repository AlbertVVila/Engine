#include "Model.h"
#include "Application.h"


Model::Model(const char * file)
{
	loadModel(file);
}

Model::~Model()
{
}

void Model::loadModel(const char * path)
{
	const aiScene* scene = aiImportFile(path, 0);
	if (scene == NULL)
	{
		LOG("ERROR importing file:%s \n", aiGetErrorString());
	}

	for (unsigned i = 0; i < scene->mNumMeshes; ++i)
	{
		GenerateMeshData(scene->mMeshes[i]);
	}

	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		GenerateMaterialData(scene->mMaterials[i]);
	}
}

void Model::GenerateMeshData(aiMesh * mesh)
{
}

void Model::GenerateMaterialData(aiMaterial * material)
{
}
