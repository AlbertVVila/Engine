#include "Application.h"

#include "Bone.h"
#include "Globals.h"

#include <assert.h>

Bone::Bone()
{
}

Bone::~Bone()
{
}

void Bone::Load(const char* boneData, unsigned uid)
{
	if (boneData == nullptr) return;

	const char* data = boneData;
	
	char* newName = new char[30];

	unsigned nameLength = *(int*)boneData;
	boneData += sizeof(int);

	for (int i = 0; i < nameLength; i++)
	{
		memcpy(newName, boneData, sizeof(char)); 
		boneName[i] = *newName;
		newName++;
		boneData += sizeof(char);
	}

	memcpy(&numVertexAffected, boneData, sizeof(int));
	boneData += sizeof(int);

	UID = uid;

	//unsigned int numIndices = *(int*)meshData;
	//meshData += sizeof(int);

	//unsigned int numVertices = *(int*)meshData;
	//meshData += sizeof(int);

	//float* vertices = (float*)meshData;
	//meshData += sizeof(float) * 3 * numVertices;

	//bool hasNormals = *(bool*)meshData;
	//meshData += sizeof(bool);

	//float* normals = nullptr;
	//if (hasNormals)
	//{
	//	normals = (float*)meshData;
	//	meshData += sizeof(float) * 3 * numVertices;
	//}

	//bool hasTexCoords = *(bool*)meshData;
	//meshData += sizeof(bool);

	//float* texCoords = nullptr;
	//if (hasTexCoords)
	//{
	//	texCoords = (float*)meshData;
	//	meshData += sizeof(float) * 2 * numVertices;
	//}

	//int* indices = (int*)meshData;
	//meshData += sizeof(int) * numIndices;

	//UID = uid;
	//this->numIndices = numIndices;
	//this->numVertices = numVertices;

	//this->vertices = new float[numVertices * 3];
	//this->indices = new int[numIndices];
	//memcpy(this->vertices, vertices, numVertices * sizeof(float) * 3);
	//memcpy(this->indices, indices, numIndices * sizeof(int));
	//ComputeBBox();
	//SetMeshBuffers(hasNormals, hasTexCoords, normals, texCoords);
	//SetBboxBuffers();
}

void Bone::Unload() 
{
	// Release bone stuff that we aint gonna need
}

