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
#include "Animation.h"

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
	std::vector<unsigned> meshesUID;
	std::vector<unsigned> animationsUID;
	std::vector<unsigned> bonesUID;

	const aiScene* scene = aiImportFile((folder+ file).c_str(), aiProcess_Triangulate);
	if (scene != nullptr)
	{
		LOG("Imported FBX %s", fbxfile);
		return ImportScene(*scene, fbxfile, meshesUID, animationsUID, bonesUID);
	}
	LOG("Error importing FBX %s", fbxfile);
	return false;
}

bool FileImporter::ImportScene(const aiScene& aiscene, const char* file, 
	std::vector<unsigned>& meshesUID, std::vector<unsigned>& animationsUID, 
	std::vector<unsigned>& bonesUID)
{

	std::vector<std::string*>* boneNames = new std::vector<std::string*>();

	std::map<unsigned, unsigned> meshMap;
	std::map<std::string*, unsigned> boneMap;

	std::vector<unsigned> rBonesUIDs;
	std::vector<Bone*> rBones;

	for (unsigned i = 0u; i < aiscene.mNumMeshes; i++)
	{
		//-------------------------------MESH------------------------------------
		unsigned meshSize = GetMeshSize(*aiscene.mMeshes[i]);
		char* meshData = new char[meshSize];

		ImportMesh(*aiscene.mMeshes[i], meshData);

		Mesh* mesh = new Mesh();
		unsigned uid = App->scene->GetNewUID();
		App->fsystem->Save((MESHES + std::to_string(uid)+ MESHEXTENSION).c_str(), meshData, meshSize);
		mesh->SetMesh(meshData, uid); //No longer deallocates data
		// meshesUID.push_back(uid); //same as below?
		App->resManager->AddMesh(mesh);
		meshMap.insert(std::pair<unsigned, unsigned>(i, mesh->UID));

		//------------------------BONES---------------------------------------
		// This needs to load the entire bone estructure and then store it as a unique resource
		//For now it loads each bone separately
		if (aiscene.mMeshes[i]->HasBones())
		{
			for (unsigned j = 0u; j < aiscene.mMeshes[i]->mNumBones; j++)
			{
				rBonesUIDs = ImportBones(*aiscene.mMeshes[i], rBones, bonesUID, boneMap, boneNames, mesh->UID);
			}
		}
		
	}
	GameObject* fake = new GameObject("fake", 0u);
	ProcessNode(meshMap, aiscene.mRootNode, &aiscene, fake, boneNames); //this function return a pointer to a GameObject? any reasons??? could be void

	//-----------------------------ANIMATIONS---------------------------------------------------
	std::map<unsigned, unsigned> animationMap;
	for (unsigned i = 0u; i < aiscene.mNumAnimations; i++) 
	{
		Animation* anim = new Animation();
		unsigned animationSize = GetAnimationSize(*aiscene.mAnimations[i]);
		char* animationData = new char[animationSize];

		ImportAnimation(*aiscene.mAnimations[i], animationData);
		anim->Load(animationData);
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

	bool hasTangents = mesh.HasTangentsAndBitangents();
	memcpy(cursor, &hasTangents, sizeof(bool));
	cursor += sizeof(bool);

	if (hasTangents)
	{
		unsigned int tangentBytes = sizeof(float)*mesh.mNumVertices * 3;
		memcpy(cursor, mesh.mTangents, tangentBytes);
		cursor += verticesBytes;
	}

}

std::vector<unsigned> FileImporter::ImportBones(const aiMesh& mesh, std::vector<Bone*> rBones,
	std::vector<unsigned>& bonesUID, std::map<std::string*, unsigned>& boneMap,
	std::vector<std::string*>* boneNames, unsigned meshUID)
{
	std::vector<unsigned> rBonesUID;

	for (int i = 0; i < mesh.mNumBones; i++)
	{
		std::vector<unsigned> mBonesUIds;

		bool boneNameFound = false;
		/*ImportSingleBone(*mesh.mBones[i], data);*/

		aiBone* bone = mesh.mBones[i];
		std::string boneName = (bone->mName.length > 0) ? bone->mName.C_Str() : "Bone"; // Use fileName+Stuff
		// Comprobar que el nombre del name no existe en el array de bones, si existe darle un +1 or sthmng
		// App->fileSystem->getAvailableNameFromArray(bonesNames, boneName);


		//std::string exportedFile;
		Bone* boneResource = new Bone(); // REMEMBER TO DELETE BONEª!

		// check if the bone is used by any animations of the fbx
		//if (App->animations->importBones(bone, UID, exportedFile))
		//{

		unsigned uid = App->scene->GetNewUID();
		mBonesUIds.push_back(uid);
		bonesUID.push_back(uid);

		std::string* nameAlloc = new std::string(boneName); 
		boneMap[nameAlloc] = uid;


		//Allocate bones names in a vector since its the only way to assosciate them to a node, pff
		if (boneNames->empty())
		{
			boneNames->push_back(nameAlloc);
		}
		else
		{
			for (const auto singleBoneName : *boneNames)
			{
				if (strcmp(nameAlloc->c_str(), singleBoneName->c_str()) == 0)
				{
					boneNameFound = true;
					break;
				}
			}
			if (!boneNameFound)
			{
				boneNames->push_back(nameAlloc);
				boneNameFound = false;
			}

		}

		unsigned boneSingleSize = GetSingleBoneSize(*mesh.mBones[i]);
		char* boneSingleData = new char[boneSingleSize];

		ImportSingleBone(*mesh.mBones[i], boneSingleData); //We import a single bone each time so we don't need to offset the data

		boneResource->Load(boneSingleData, uid, meshUID); // Load all the data from the cursor
			
		boneResource->boneName = boneName;

		// if bone exists inside you go
		if (boneResource != nullptr)
		{
			rBonesUID.push_back(boneResource->UID);
			rBones.push_back(boneResource);
		}


		//Bone* bone = new Bone();
		//unsigned boneSingleSize = GetSingleBoneSize(*aiscene.mMeshes[i]->mBones[j]);
		//char* boneSingleData = new char[boneSingleSize];
		//ImportSingleBone(*aiscene.mMeshes[i]->mBones[j], boneSingleData); //We import a single bone each time so we don't need to offset the data
		//uid = App->scene->GetNewUID(); // every bone needs to have his own UID? 
		////App->fsystem->Save((BONES + std::to_string(uid) + BONEEXTENSION).c_str(), bonesData, bonesSize); //Is this ok? :'D created extensions and stuff
		//bone->Load(boneSingleData, uid);
		//// App->resManager->AddBone(mesh); //Do we need the ress manager to load every bone?
		//// meshMap.insert(std::pair<unsigned, unsigned>(i, mesh->UID))
	}
	return rBonesUID;
}

//For now it only saves the Name, for testing purposes, WIP saving all the other vbles
void FileImporter::ImportSingleBone(const aiBone& bone, char* data)
{
	char* cursor = data;

	//Allocates the size of the string name
	memcpy(cursor, &bone.mName.length, sizeof(int));
	cursor += sizeof(int);

	memcpy(cursor, bone.mName.C_Str(), sizeof(char) * bone.mName.length);  //Name
	cursor += sizeof(char) * bone.mName.length;

	memcpy(cursor, &bone.mNumWeights, sizeof(int));
	cursor += sizeof(int);

	for (unsigned i = 0u; i < bone.mNumWeights; i++)
	{
		memcpy(cursor, &bone.mWeights[i].mVertexId, sizeof(int));
		cursor += sizeof(int);

		memcpy(cursor, &bone.mWeights[i].mWeight, sizeof(float));
		cursor += sizeof(float);
	}

	memcpy(cursor, &bone.mOffsetMatrix, sizeof(math::float4x4));
	cursor += sizeof(math::float4x4);
}


void FileImporter::ImportAnimation(const aiAnimation& animation, char* data) 
{
	char* cursor = data;

	memcpy(cursor, &animation.mDuration, sizeof(double));
	cursor += sizeof(double);

	memcpy(cursor, &animation.mTicksPerSecond, sizeof(double));
	cursor += sizeof(double);

	memcpy(cursor, &animation.mNumChannels, sizeof(int));
	cursor += sizeof(int);
	//TODO: as ImportBones
}

//TODO: Obtain animations size in order to store their content
unsigned FileImporter::GetAnimationSize(const aiAnimation& animation) const 
{
	unsigned size = 0u;

	size += sizeof(double) * 2 + sizeof(int); //Duration, ticks per second and number of channels


	//if (animation.mNumChannels < 0u)
	//{
	//	size += sizeof(aiNodeAnim) * animation.mNumChannels;
	//}

	return size;
}

unsigned FileImporter::GetMeshSize(const aiMesh &mesh) const
{
	unsigned size = 0u;
	unsigned int ranges[2] = { mesh.mNumFaces * 3, mesh.mNumVertices };
	size += sizeof(ranges); //numfaces + numvertices
	size += ranges[0]* 3 * sizeof(int); //indices

	size += sizeof(float)*ranges[1] * 3; //vertices
	size += sizeof(bool) * 3; //has normals + has tcoords + has tangents
	if (mesh.HasNormals())
	{
		size += sizeof(float)*ranges[1] * 3;
	}
	if (mesh.HasTextureCoords(0))
	{
		size += sizeof(float)*ranges[1] * 2;
	}
	if (mesh.HasTangentsAndBitangents())
	{
		size += sizeof(float)*ranges[1] * 3;
	}

	return size;
}

unsigned FileImporter::GetBonesSize(const aiMesh& mesh) const
{

	//Bones
	unsigned size = 0u;

	if (mesh.HasBones())
	{
		for (int i = 1; i < mesh.mNumBones; i++)
		{
			size += GetSingleBoneSize(*mesh.mBones[i]);
		}
	}

	return size;
}

unsigned FileImporter::GetSingleBoneSize(const aiBone& bone) const
{
	unsigned size = 0u;

	size += sizeof(int) + sizeof(char) * bone.mName.length; //name and its size

	size += sizeof(int) + (sizeof(int) + sizeof(float)) * bone.mNumWeights; //Number of affected vertex + their id and weights

	size += sizeof(math::float4x4); //Offset matrix of the bone

	return size;
}

GameObject* FileImporter::ProcessNode(const std::map<unsigned, unsigned> &meshmap,
	const aiNode * node, const aiScene * scene, GameObject* parent, std::vector<std::string*>* boneNames)
{
	assert(node != nullptr);
	if (node == nullptr) return nullptr;

	GameObject * gameObject = nullptr;
	GameObject * meshGO = nullptr;
	
	std::vector<GameObject*> gameobjects; 
	gameobjects.push_back(gameObject);

	for (unsigned k = 0u; k < node->mNumChildren; k++) //Splits meshes of same node into diferent gameobjects 
	{
		//comparar nombre node con array de bones, si concuerda generamos GO, si no a casa

		for (std::vector<std::string*>::iterator boneName = boneNames->begin(); boneName != boneNames->end(); ++boneName)  //this creates the GOs correctly now, it's missing the link to the bone
		{
			if (strcmp(node->mName.C_Str(), (*boneName)->c_str() ) == 0) 
			{
				gameObject = App->scene->CreateGameObject(node->mName.C_Str(), parent); //Creating GO inside the conditional now

				aiMatrix4x4 m = node->mTransformation;
				float4x4 transform(m.a1, m.a2, m.a3, m.a4, m.b1, m.b2, m.b3, m.b4, m.c1, m.c2, m.c3, m.c4, m.d1, m.d2, m.d3, m.d4);
				ComponentTransform* t = (ComponentTransform *)gameObject->CreateComponent(ComponentType::Transform);
				t->AddTransform(transform);

				(*boneName)->erase(0); //erases the name on the boneNames array, it would be better to delete the position but iterator suffers
			}
		}			
	}
	

	//This below WORKS, but it's not optimal since we are creating two GOs at the same time, needs a lot more work but for now Meshes and bones imported

	for (unsigned j = 0u; j < node->mNumMeshes; j++) //Splits meshes of same node into diferent gameobjects 
	{
		meshGO = App->scene->CreateGameObject(node->mName.C_Str(), parent);

		aiMatrix4x4 m = node->mTransformation;
		float4x4 transform(m.a1, m.a2, m.a3, m.a4, m.b1, m.b2, m.b3, m.b4, m.c1, m.c2, m.c3, m.c4, m.d1, m.d2, m.d3, m.d4);
		ComponentTransform* t = (ComponentTransform *)meshGO->CreateComponent(ComponentType::Transform);
		t->AddTransform(transform);

		gameobjects.push_back(meshGO);
		meshGO->parent = parent;
		ComponentRenderer* crenderer = (ComponentRenderer*)meshGO->CreateComponent(ComponentType::Renderer);
		auto it = meshmap.find(node->mMeshes[j]);
		if (it != meshmap.end())
		{
			RELEASE(crenderer->mesh);
			crenderer->mesh = App->resManager->GetMesh(it->second);
			meshGO->UpdateBBox();
		}
	}

	//for (unsigned i = 0u; i < node->mNumMeshes; i++)
	//{
	//	ComponentRenderer* crenderer = (ComponentRenderer*)gameobjects[i]->CreateComponent(ComponentType::Renderer);

	//}

	//To simplify hierarchy, if in this iteration GO was not created and we didnt have this, it would give an error next iteration since next parent would be nullptr

	if (gameObject == nullptr)
	{
		gameObject = parent;
	}
	

	for (unsigned i = 0u; i < node->mNumChildren; i++)
	{
		GameObject * child = ProcessNode(meshmap, node->mChildren[i], scene, gameObject, boneNames); //We don't use this child anywhere!!!!!
	}

	return gameObject; //WHYYYYYYYYYY?"?"???!!?
}


