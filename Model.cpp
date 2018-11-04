#include "Model.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleProgram.h"

Model::Model(const char * file)
{
	std::string s(file);
	std::size_t found = s.find_last_of("/\\");
	s = s.substr(0, found+1);
	this->path = s.c_str();
	LoadModel(file);
}

Model::~Model()
{
}

void Model::LoadModel(const char * path)
{
	const aiScene* scene = aiImportFile(path, aiProcess_Triangulate);
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

	std::string texturePath(path);
	texturePath += file.C_Str();
	unsigned int texture = App->textures->Load(texturePath.c_str());

	textures.push_back(texture);
}

void Model::Draw() const
{
	for (auto &mesh : meshes)
	{
		mesh.Draw(App->program->shaderProgram, textures);
	}
}
