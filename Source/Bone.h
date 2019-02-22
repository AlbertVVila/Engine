#ifndef __BONE_H__
#define __BONE_H__

#include "Math/float4x4.h"

class Bone
{
	public:
		Bone();
		~Bone();

	public:
		void Load(const char* meshData, unsigned uid);
		void Unload();
	public:
		char boneName[30];
		unsigned UID = 0;
		unsigned numVertexAffected = 0;
		int* vertexId = nullptr;
		float* vertexWeight = nullptr;
		float4x4 offsetMatrix = float4x4::zero;
};

#endif // __BONE_H__
