#include "Application.h"

#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ModuleResourceManager.h"

#include "GameObject.h"
#include "ComponentRenderer.h"
#include "FileImporter.h"

#include "Material.h"
#include "Mesh.h"

#include <assert.h>
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/mesh.h"
#include "assimp/material.h"
#include "assimp/types.h"
#include <map>

void AddLog(const char* str, char* userData)
{
	std::string info(str);
	info.pop_back();

	LOG("%s", info.c_str());
}

FileImporter::FileImporter()
{
	struct aiLogStream streamLog;
	streamLog = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, NULL);
	streamLog.callback = AddLog;
	aiAttachLogStream(&streamLog);
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
	else if (extension == ".png" || extension == ".jpg" || extension == ".tif")
	{
		App->textures->ImportImage(file, folder);
	}
	else if (extension == TEXTUREEXT)
	{
		App->fsystem->Copy(folder, TEXTURES, file);
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

bool FileImporter::ImportScene(const aiScene &scene, const char* file)
{
	std::map<unsigned, unsigned> meshMap;
	for (unsigned i = 0; i < scene.mNumMeshes; i++)
	{
		unsigned size = GetMeshSize(*scene.mMeshes[i]);
		char* data = new char[size];
		ImportMesh(*scene.mMeshes[i], data);

		Mesh *mesh = new Mesh();
		mesh->SetMesh(data, App->scene->GetNewUID());
		App->resManager->AddMesh(mesh);
		App->fsystem->Save((MESHES + std::to_string(mesh->UID)+ MESHEXTENSION).c_str(), data, size);
		meshMap.insert(std::pair<unsigned, unsigned>(i, mesh->UID));
	}
	GameObject *fake = new GameObject("fake",0);
	ProcessNode(meshMap, scene.mRootNode, &scene, scene.mRootNode->mTransformation, fake);
	App->scene->SaveScene(*fake, App->fsystem->GetFilename(file).c_str()); //TODO: Make AutoCreation of folders or check
	//TODO: CleanUP on ending import and on saving
	return true; //TODO: Load Clear Scene
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

	bool hasNormals = mesh.HasNormals();
	memcpy(cursor, &hasNormals, sizeof(bool));
	cursor += sizeof(bool);

	if (hasNormals)
	{
		unsigned int normalBytes = sizeof(float)*mesh.mNumVertices * 3;
		memcpy(cursor, mesh.mNormals, normalBytes);
		cursor += verticesBytes;
	}

	bool hasTextureCoords = mesh.HasTextureCoords(0);
	memcpy(cursor, &hasTextureCoords, sizeof(bool));
	cursor += sizeof(bool);

	if (hasTextureCoords)
	{
		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			memcpy(cursor, &mesh.mTextureCoords[0][i].x, sizeof(float));
			cursor += sizeof(float);
			memcpy(cursor, &mesh.mTextureCoords[0][i].y, sizeof(float));
			cursor += sizeof(float);
		}
	}

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

	size += sizeof(float)*ranges[1] * 3; //vertices
	size += sizeof(bool) * 2; //has normals + has tcoords
	if (mesh.HasNormals())
	{
		size += sizeof(float)*ranges[1] * 3;
	}
	if (mesh.HasTextureCoords(0))
	{
		size += sizeof(float)*ranges[1] * 2;
	}
	return size;
}


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
		ComponentRenderer* crenderer = (ComponentRenderer*)gameobjects[i]->CreateComponent(ComponentType::Renderer);//TODO: avoid map and use resource manager
		auto it = meshmap.find(node->mMeshes[i]);
		if (it != meshmap.end())
		{
			crenderer->mesh = App->resManager->GetMesh(it->second);;
		}

		aiMaterial * mat = scene->mMaterials[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex];
		aiTextureMapping mapping = aiTextureMapping_UV;
		for (unsigned i = 1; i <= 4; i++) //Gets diffuse,specular,occlusion and emissive
		{
			aiString texture;
			mat->GetTexture((aiTextureType)i, 0, &texture, &mapping, 0);
			if (texture.length > 0)
			{
				crenderer->SetMaterial(DEFAULTMAT);
				crenderer->material->textures[i] = new Texture(App->fsystem->GetFilename(texture.C_Str()));// TODO: SOBRESCRIUS DEFAULT!!
			}
		}
	} 
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		GameObject * child = ProcessNode(meshmap, node->mChildren[i], scene, transform, gameobject);
	}
	return gameobject; //TODO: create a small resource manager for meshes/materials etc
}


