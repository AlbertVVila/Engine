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
	assert(path != NULL);
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
	GetBoundingBox();
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

void Model::UpdateTexture(unsigned int texture)
{
	for (auto &Oldtexture : textures)
	{
		Oldtexture = texture;
	}
}

void Model::GetBoundingBox()
{
	if (meshes.size() == 0 || meshes.front().vertices.size() == 0) return;

	float3 min, max;
	min = max = meshes.front().vertices[0];
	for (auto &mesh : meshes)
	{
		for (auto &vertice : mesh.vertices)
		{
			min.x = MIN(min.x, vertice.x);
			min.y = MIN(min.y, vertice.y);
			min.z = MIN(min.z, vertice.z);

			max.x = MAX(max.x, vertice.x);
			max.y = MAX(max.y, vertice.y);
			max.z = MAX(max.z, vertice.z);

		}
	}
	BoundingBox.minPoint = min;
	BoundingBox.maxPoint = max;
}