#include "Bone.h"

#include "Globals.h"
#include <assert.h>

Bone::Bone()
{
}

Bone::~Bone()
{
}

void Bone::Load(const char* meshData, unsigned uid)
{
	if (meshData == nullptr) return;

	const char* data = meshData;

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

	RELEASE_ARRAY(data);
}

void Bone::Unload() {
	// Release bone stuff that we aint gonna need
}

