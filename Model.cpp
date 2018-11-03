#include "Model.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleProgram.h"


Model::Model(const char * file)
{
	LoadModel(file);
}

Model::~Model()
{
}

void Model::LoadModel(const char * path)
{
	const aiScene* scene = aiImportFile(path, 0);
	if (scene == NULL)
	{
		LOG("ERROR importing file:%s \n", aiGetErrorString());
	}

	for (unsigned i = 0; i < scene->mNumMeshes; ++i)
	{
		meshes.emplace_back(scene->mMeshes[i]);
	}

	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		GenerateMaterialData(scene->mMaterials[i]);
	}
}
	

void Model::GenerateMaterialData(aiMaterial * material)
{
	aiTextureMapping mapping = aiTextureMapping_UV;
	aiString file;
	material->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, 0);
	unsigned int texture = App->textures->Load(file.C_Str());
	textures.push_back(texture);

}

void Model::Draw() const
{
	for (auto &mesh : meshes)
	{
		mesh.Draw(App->program->shaderProgram, textures);
	}
}
