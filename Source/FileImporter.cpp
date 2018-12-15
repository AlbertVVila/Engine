#include "FileImporter.h"
#include <assert.h>
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/mesh.h"
#include "assimp/material.h"
#include "assimp/types.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include <map>
#include "ComponentMaterial.h"
#include "ComponentMesh.h"

FileImporter::FileImporter()
{
}


FileImporter::~FileImporter()
{
}

void FileImporter::ImportAsset(const char *file, const char *folder)  //TODO:files logs
{
	std::string extension (App->fsystem->GetExtension(file));
	if (extension == FBXEXTENSION || extension == ".FBX")
	{
		ImportFBX(file, folder);
	}
	else if (extension == ".png" || extension == ".jpg")
	{
		App->textures->ImportImage(file, folder);
	}
	else if (extension == TEXTUREEXT)
	{
		App->fsystem->Copy(folder, TEXTURES, file); //TODO: FULL PATH when copying outside fs
	}
	else if (extension == MESHEXTENSION)
	{
		App->fsystem->Copy(folder, MESHES, file);
	}
}

bool FileImporter::ImportFBX(const char* fbxfile, const char* folder)
{
	assert(fbxfile != nullptr);
	if (fbxfile == nullptr) return false;
	std::string file(fbxfile);
	const aiScene* scene = aiImportFile((folder+ file).c_str(), aiProcess_Triangulate);
	if (scene != nullptr)
	{
		return ImportScene(*scene, fbxfile);
	}
	return false;
}

bool FileImporter::ImportScene(const aiScene &scene, const char* file) //TODO: move everything to sceneLoader?
{
	std::map<unsigned, unsigned> meshMap;
	for (unsigned i = 0; i < scene.mNumMeshes; i++)
	{
		unsigned size = GetMeshSize(*scene.mMeshes[i]);
		char* data = new char[size];
		ImportMesh(*scene.mMeshes[i], data);

		unsigned meshUID = App->scene->GetNewUID();
		meshMap.insert(std::pair<unsigned, unsigned>(i, meshUID));

		App->fsystem->Save((MESHES + std::to_string(meshUID)+ MESHEXTENSION).c_str(), data, size);
	}
	GameObject *fake = new GameObject("fake",0);
	ProcessNode(meshMap, scene.mRootNode, &scene, scene.mRootNode->mTransformation, fake);
	App->scene->SaveScene(*fake, App->fsystem->GetFilename(file).c_str()); //TODO: Make AutoCreation of folders or check
	//TODO: CleanUP on ending import and on saving
	return true; //TODO: Load specific scene, Save specific scene, Clear
}

void FileImporter::ImportMesh(const aiMesh &mesh, char *data)
{
	char *cursor = data;

	unsigned ranges[2] = { mesh.mNumFaces * 3, 	mesh.mNumVertices };
	unsigned rangeBytes = sizeof(ranges);
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

unsigned FileImporter::GetMeshSize(const aiMesh &mesh)
{
	unsigned size = 0u;
	unsigned int ranges[2] = { mesh.mNumFaces * 3, mesh.mNumVertices };
	//size += sizeof(int); //mesh content size ?
	size += sizeof(ranges); //numfaces + numvertices
	size += ranges[0]* 3 * sizeof(int); //indices
	size += sizeof(float)*ranges[1] * 5; //vertices + texCoords
	return size;
}


//TODO: we could optimize on FBX import
GameObject* FileImporter::ProcessNode(const std::map<unsigned, unsigned> &meshmap, const aiNode * node, const aiScene * scene, const aiMatrix4x4 & parentTransform, GameObject* parent)
{
	assert(node != nullptr); assert(scene != nullptr);
	aiMatrix4x4 transform = parentTransform * node->mTransformation; //TODO: transform conversion
	GameObject * gameobject = App->scene->CreateGameObject(float4x4::identity, "", node->mName.C_Str(), parent); //TODO: remove deprecated fbxfile variable

	std::vector<GameObject*> gameobjects;
	gameobjects.push_back(gameobject);
	for (unsigned k = 1; k < node->mNumMeshes; k++)
	{
		GameObject *copy = new GameObject(*gameobject);
		gameobjects.push_back(copy);
		copy->parent = gameobject->parent;
		parent->children.push_back(copy);
	}

	for (unsigned i = 0; i < node->mNumMeshes; i++)
	{
		ComponentMesh* mesh = (ComponentMesh*)gameobjects[i]->CreateComponent(ComponentType::Mesh);
		auto it = meshmap.find(node->mMeshes[i]);
		if (it != meshmap.end())
		{
			char *data;
			App->fsystem->Load((MESHES + std::to_string(it->second) + MESHEXTENSION).c_str(), &data);
			mesh->SetMesh(data);
			mesh->meshUID = it->second;
		}

		ComponentMaterial* material = (ComponentMaterial*)gameobjects[i]->CreateComponent(ComponentType::Material); //TODO: avoid map and use resource manager
		aiMaterial * mat = scene->mMaterials[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex];
		aiTextureMapping mapping = aiTextureMapping_UV;
		aiString file;
		mat->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, 0);
		if (file.length > 0)
		{
			material->textureFile = App->fsystem->GetFilename(file.C_Str()); //we only save texture name
		}
	} 
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		GameObject * child = ProcessNode(meshmap, node->mChildren[i], scene, transform, gameobject);
	}
	return gameobject; //TODO: create a small resource manager for meshes/materials etc
}

