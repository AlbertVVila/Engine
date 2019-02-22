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
	
	//Bone name
	
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

	//Number of vertex affected
	memcpy(&numVertexAffected, boneData, sizeof(int));
	boneData += sizeof(int);

	//Weights and Ids of such vertexes
	vertexId = new int[numVertexAffected];
	vertexWeight = new float[numVertexAffected];

	for (int i = 0; i < numVertexAffected; i++)
	{
		memcpy(vertexId, boneData, sizeof(int));
		boneData += sizeof(int);
		vertexId++;

		memcpy(vertexWeight, boneData, sizeof(float));
		boneData += sizeof(float);
		vertexWeight++;
	}

	//Offset matrix for the bone
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			memcpy(&offsetMatrix[i][j], boneData, sizeof(float));
			boneData += sizeof(float);
		}
	}

	UID = uid;
}

void Bone::Unload() 
{
	// Release bone stuff that we aint gonna need
}

