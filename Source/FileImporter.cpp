#include "Application.h"

#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ModuleResourceManager.h"

#include "GameObject.h"
#include "ComponentRenderer.h"
#include "ComponentTransform.h"
#include "ComponentAnimation.h"

#include "FileImporter.h"
#include "Material.h"
#include "Mesh.h"
#include "Animation.h"

#include <assert.h>
#include <stack>
#include "Math/float4x4.h"
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
	else if (extension == ANIMATIONEXTENSION)
	{
		App->fsystem->Copy(folder, ANIMATIONS, file);
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
	GameObject* sceneGO = App->scene->CreateGameObject("Scene", App->scene->root);
	sceneGO->CreateComponent(ComponentType::Transform); // To move all around
	sceneGO->isBoneRoot = true;
	std::stack<aiNode*> stackNode;
	std::stack<GameObject*> stackParent;

	for (unsigned i = 0; i < aiscene.mRootNode->mNumChildren; ++i) //skip rootnode
	{
		stackNode.push(aiscene.mRootNode->mChildren[i]);
		stackParent.push(sceneGO);
	}

	while (!stackNode.empty())
	{
		aiNode* aNode = stackNode.top(); stackNode.pop();
		GameObject* goNode = new GameObject(aNode->mName.C_Str(), App->scene->GetNewUID());
		stackParent.top()->InsertChild(goNode); stackParent.pop();
		
		ComponentTransform* nodeTransform = (ComponentTransform*)goNode->CreateComponent(ComponentType::Transform);
		ComponentTransform* nodeParentTransform = (ComponentTransform*)goNode->parent->GetComponent(ComponentType::Transform);
		nodeTransform->UpdateTransform();
		nodeTransform->SetLocalTransform(reinterpret_cast<const math::float4x4&>(aNode->mTransformation), nodeParentTransform->global);		
		for (unsigned i = 0u; i < aNode->mNumMeshes; ++i)
		{
			aiMesh* aMesh = aiscene.mMeshes[aNode->mMeshes[i]];
			unsigned meshSize = GetMeshSize(*aMesh);
			char* meshData = new char[meshSize];

			ImportMesh(*aMesh, meshData);

			Mesh* mesh = new Mesh();
			unsigned meshUid = App->scene->GetNewUID();
			App->fsystem->Save((MESHES + std::to_string(meshUid) + MESHEXTENSION).c_str(), meshData, meshSize);
			mesh->SetMesh(meshData, meshUid);
			RELEASE_ARRAY(meshData);
			App->resManager->AddMesh(mesh);
			GameObject* meshGO = new GameObject();
			ComponentRenderer* crenderer = (ComponentRenderer*)meshGO->CreateComponent(ComponentType::Renderer);
			crenderer->mesh = mesh;
			meshGO->CreateComponent(ComponentType::Transform);
			meshGO->name = "Mesh";
			goNode->InsertChild(meshGO);
		}

		for (unsigned i = 0u; i < aNode->mNumChildren; ++i)
		{
			stackNode.push(aNode->mChildren[i]);
			stackParent.push(goNode);
		}
	}

	for (unsigned i = 0u; i < aiscene.mNumAnimations; i++)
	{
		char* animationData = nullptr;
		unsigned animationSize = sizeof(double) * 2 + sizeof(int) + aiscene.mAnimations[i]->mDuration * aiscene.mAnimations[i]->mNumChannels * sizeof(Channel);
		animationData = new char[animationSize];
		
		ImportAnimation(*aiscene.mAnimations[i], animationData);

		sceneGO->CreateComponent(ComponentType::Animation);
		ComponentAnimation* animationComponent = (ComponentAnimation*)sceneGO->GetComponent(ComponentType::Animation);
		unsigned animUid = App->scene->GetNewUID();
		Animation* animation = new Animation();
		animation->Load(animationData, animUid);

		animation->animationName = aiscene.mAnimations[i]->mName.C_Str();

		App->fsystem->Save((ANIMATIONS + std::to_string(animUid) + ANIMATIONEXTENSION).c_str(), animationData, animationSize);

		App->resManager->AddAnim(animation);

		animationComponent->anim = animation;

		RELEASE_ARRAY(animationData);
	}
	
	App->scene->SaveScene(*sceneGO, *App->fsystem->GetFilename(file).c_str(), *SCENES); //TODO: Make AutoCreation of folders or check

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

	//------------------------BONES---------------------------------------
	if (mesh.HasBones())
	{
		memcpy(cursor, &mesh.mNumBones, sizeof(unsigned));
		cursor += sizeof(unsigned);
		ImportBones(mesh.mBones, mesh.mNumBones, cursor);
	}
	else
	{
		memcpy(cursor, (unsigned)0u, sizeof(unsigned));
		cursor += sizeof(unsigned);
	}

}

void FileImporter::ImportBones(aiBone** bones, unsigned numBones, char* data)const
{
	char* cursor = data;

	for (unsigned i = 0u; i < numBones; ++i)
	{
		memcpy(cursor, bones[i]->mName.C_Str(), MAX_BONE_NAME_LENGTH);
		cursor += MAX_BONE_NAME_LENGTH;
		
		math::float4x4 boneOffset = reinterpret_cast<const math::float4x4&>(bones[i]->mOffsetMatrix);
		
		
		memcpy(cursor, &boneOffset[0][0], sizeof(math::float4x4));
		cursor += sizeof(math::float4x4);

		memcpy(cursor, &bones[i]->mNumWeights, sizeof(unsigned));
		cursor += sizeof(unsigned);

		for (unsigned j = 0u; j < bones[i]->mNumWeights; ++j)
		{
			memcpy(cursor, &bones[i]->mWeights[j].mVertexId, sizeof(unsigned));
			cursor += sizeof(unsigned);
			memcpy(cursor, &bones[i]->mWeights[j].mWeight, sizeof(float));
			cursor += sizeof(unsigned);
		}
	}
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

	for (unsigned j = 0u; j < animation.mNumChannels; j++)
	{
		memcpy(cursor, animation.mChannels[j]->mNodeName.C_Str(), sizeof(char) * MAX_BONE_NAME_LENGTH);  //Name
		cursor += sizeof(char) * MAX_BONE_NAME_LENGTH;

		memcpy(cursor, &animation.mChannels[j]->mNumPositionKeys, sizeof(int));
		cursor += sizeof(int);

		memcpy(cursor, &animation.mChannels[j]->mNumRotationKeys, sizeof(int));
		cursor += sizeof(int);

		//importar longitud array de posiciones e iterar

		for (unsigned i = 0u; i < animation.mChannels[j]->mNumPositionKeys; i++)
		{
			memcpy(cursor, &animation.mChannels[j]->mPositionKeys[i].mValue, sizeof(float) * 3);
			cursor += sizeof(float) * 3;
		}

		//importar longitud array de rotaciones e iterar

		for (unsigned k = 0u; k < animation.mChannels[j]->mNumRotationKeys; k++)
		{
			memcpy(cursor, &animation.mChannels[j]->mRotationKeys[k].mValue, sizeof(math::Quat));
			cursor += sizeof(math::Quat);
		}
	}
}

//TODO: Obtain animations size in order to store their content
unsigned FileImporter::GetAnimationSize(const aiAnimation& animation) const 
{
	unsigned size = 0u;

	size += sizeof(double) * 2 + sizeof(int); //Duration, ticks per second and number of channels

	size += sizeof(int);

	for (unsigned j = 0u; j < animation.mNumChannels; j++)
	{
		size += sizeof(char) * MAX_BONE_NAME_LENGTH;

		size += sizeof(int) * 2;

		for (unsigned i = 0u; i < animation.mChannels[j]->mNumPositionKeys; i++)
		{
			size += sizeof(float) * 3;
		}

		for (unsigned i = 0u; i < animation.mChannels[j]->mNumRotationKeys; i++)
		{
			size += sizeof(math::Quat);
		}
	}

	return size;
}

unsigned FileImporter::GetMeshSize(const aiMesh &mesh) const
{
	unsigned size = 0u;
	unsigned int ranges[2] = { mesh.mNumFaces * 3, mesh.mNumVertices };
	size += sizeof(ranges); //numfaces + numvertices
	size += ranges[0] * 3 * sizeof(int); //indices

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
	if (mesh.HasBones())
	{
		unsigned boneSize = sizeof(char) * MAX_BONE_NAME_LENGTH;
		boneSize += sizeof(unsigned);
		boneSize += sizeof(math::float4x4);
		boneSize += mesh.mNumVertices * (sizeof(unsigned) + sizeof(float));
		size += mesh.mNumBones * boneSize;
	}
	return size;
}




