#include "Application.h"

#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ModuleResourceManager.h"

#include "GameObject.h"
#include "ComponentRenderer.h"
#include "ComponentTransform.h"

#include "FileImporter.h"
#include "Material.h"
#include "Mesh.h"
#include "Bone.h"

#include <assert.h>
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/mesh.h"
#include "assimp/material.h"

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

void FileImporter::ImportAsset(const char *file, const char *folder)
{
	std::string extension (App->fsystem->GetExtension(file));
	if (extension == FBXEXTENSION || extension == FBXCAPITAL)
	{
		ImportFBX(file, folder);
	}
	else if (extension == PNG || extension == TIF || extension == JPG)
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
		LOG("Imported FBX %s", fbxfile);
		return ImportScene(*scene, fbxfile);
	}
	LOG("Error importing FBX %s", fbxfile);
	return false;
}

bool FileImporter::ImportScene(const aiScene& aiscene, const char* file)
{
	std::map<unsigned, unsigned> meshMap;
	for (unsigned i = 0; i < aiscene.mNumMeshes; i++)
	{
		//Separated data for both Bones and Mesh, data is now independent from one another
		unsigned meshSize = GetMeshSize(*aiscene.mMeshes[i]);
		unsigned bonesSize = GetBonesSize(*aiscene.mMeshes[i]);
		char* meshData = new char[meshSize];
		char* bonesData = new char[bonesSize];

		//Imports Mesh and Bones separately
		ImportMesh(*aiscene.mMeshes[i], meshData);
		ImportBones(*aiscene.mMeshes[i], bonesData);

		Mesh* mesh = new Mesh();
		unsigned uid = App->scene->GetNewUID();
		App->fsystem->Save((MESHES + std::to_string(uid)+ MESHEXTENSION).c_str(), meshData, meshSize);
		mesh->SetMesh(meshData, uid); //No longer deallocates data
		App->resManager->AddMesh(mesh);
		meshMap.insert(std::pair<unsigned, unsigned>(i, mesh->UID));

		// This needs to load the entire bone estructure and then store it as a unique resource
		for (unsigned j = 0u; j < aiscene.mMeshes[i]->mNumBones; j++)
		{
			Bone* bone = new Bone();
			uid = App->scene->GetNewUID(); // every bone needs to have his own UID? 
			//App->fsystem->Save((BONES + std::to_string(uid) + BONEEXTENSION).c_str(), bonesData, bonesSize); //Is this ok? :'D created extensions and stuff
			bone->Load(bonesData, uid);
			// App->resManager->AddBone(mesh); //Do we need the ress manager to load every bone?
			// meshMap.insert(std::pair<unsigned, unsigned>(i, mesh->UID));
		}
	}
	GameObject* fake = new GameObject("fake", 0u);
	ProcessNode(meshMap, aiscene.mRootNode, &aiscene, fake);

	std::map<unsigned, unsigned> animationMap;
	for (unsigned i = 0u; i < aiscene.mNumAnimations; i++) 
	{
		// ImportAnimation(*aiscene.mAnimations[i], data);
	}

	App->scene->SaveScene(*fake, *App->fsystem->GetFilename(file).c_str(), *SCENES); //TODO: Make AutoCreation of folders or check
	fake->CleanUp();
	RELEASE(fake);

	aiReleaseImport(&aiscene);
	return true;
}

void FileImporter::ImportMesh(const aiMesh& mesh, char* data)
{
	char* cursor = data;

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

//For now it only saves the Name, for testing purposes, WIP saving all the other vbles
void FileImporter::ImportBones(const aiMesh& mesh, char* data)
{
	char* cursor = data;

	for (unsigned i = 0u; i < mesh.mNumBones; i++)
	{
		aiBone* bone = mesh.mBones[i];

		//Allocates the size of the string name
		memcpy(cursor, &bone->mName.length, sizeof(int));
		cursor += sizeof(int);

		memcpy(cursor, bone->mName.C_Str(), sizeof(char) * bone->mName.length);  //Name
		cursor += sizeof(char) * bone->mName.length;
		//memcpy(cursor, &bone->mNumWeights, sizeof(unsigned));  //numWieghts
		//cursor += sizeof(unsigned);
		//memcpy(cursor, &bone->mOffsetMatrix, sizeof(aiMatrix4x4));  //offsetmatrix
		//cursor += sizeof(aiMatrix4x4);

		//for (unsigned j = 0u; j < bone->mNumWeights; j++)					//wieghts
		//{
		//	memcpy(cursor, &bone->mWeights[j], sizeof(aiVertexWeight));  //weights
		//	cursor += sizeof(aiVertexWeight);

		/*	memcpy(cursor, &bone->mWeights->mVertexId, sizeof(unsigned));
		//	cursor += sizeof(unsigned);
		//	memcpy(cursor, &bone->mWeights->mWeight, sizeof(float));
		//	cursor += sizeof(float);*/
		//}	
	}

}


void FileImporter::ImportAnimation(const aiAnimation& animation, char* data) {
	//TODO: as ImportMeshAndBones
}

//TODO: Obtain animations size in order to store their content
unsigned FileImporter::GetAnimationSize(const aiAnimation& animation) const {
	unsigned size = 0u;
	//unsigned int ranges[2] = { animation.mNumChannels, animation.mNumMeshChannels};
	//size += sizeof(ranges); //numfaces + numvertices
	//size += ranges[0] * 3 * sizeof(int); //indices

	//size += sizeof(float)*ranges[1] * 3; //vertices
	//size += sizeof(bool) * 2; //has normals + has tcoords
	if (animation.mNumChannels < 0u)
	{
		size += sizeof(aiNodeAnim) * animation.mNumChannels;
	}

	return size;
}

unsigned FileImporter::GetMeshSize(const aiMesh &mesh) const
{
	unsigned size = 0u;
	unsigned int ranges[2] = { mesh.mNumFaces * 3, mesh.mNumVertices };
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

unsigned FileImporter::GetBonesSize(const aiMesh &mesh) const
{

	//Bones
	unsigned size = 0u;

	if (mesh.HasBones())
	{
		for (int i = 1; i < mesh.mNumBones; i++)
		{
			size += sizeof(int) + sizeof(char) * mesh.mBones[i]->mName.length;
		}
	}

	return size;
}



GameObject* FileImporter::ProcessNode(const std::map<unsigned, unsigned> &meshmap, const aiNode * node, const aiScene * scene, GameObject* parent)
{
	assert(node != nullptr);
	if (node == nullptr) return nullptr;

	GameObject * gameobject = App->scene->CreateGameObject(node->mName.C_Str(), parent);

	aiMatrix4x4 m = node->mTransformation;
	float4x4 transform(m.a1, m.a2, m.a3, m.a4, m.b1, m.b2, m.b3, m.b4, m.c1, m.c2, m.c3, m.c4, m.d1, m.d2, m.d3, m.d4);
	ComponentTransform* t = (ComponentTransform *)gameobject->CreateComponent(ComponentType::Transform);
	t->AddTransform(transform);

	std::vector<GameObject*> gameobjects;
	gameobjects.push_back(gameobject);
	for (unsigned k = 1; k < node->mNumMeshes; k++) //Splits meshes of same node into diferent gameobjects 
	{
		GameObject *copy = new GameObject(*gameobject);
		gameobjects.push_back(copy);
		copy->parent = gameobject->parent;
		parent->children.push_back(copy);
	}

	for (unsigned i = 0; i < node->mNumMeshes; i++)
	{
		ComponentRenderer* crenderer = (ComponentRenderer*)gameobjects[i]->CreateComponent(ComponentType::Renderer);
		auto it = meshmap.find(node->mMeshes[i]);
		if (it != meshmap.end())
		{
			RELEASE(crenderer->mesh);
			crenderer->mesh = App->resManager->GetMesh(it->second);
			gameobjects[i]->UpdateBBox();
		}
	} 
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		GameObject * child = ProcessNode(meshmap, node->mChildren[i], scene, gameobject);
	}
	return gameobject;
}


