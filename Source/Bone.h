#ifndef __BONE_H__
#define __BONE_H__

#include "Math/float4x4.h"

#include <vector>

class Bone
{
	public:
		Bone();
		~Bone();

	public:
		void Load(const char* boneData, unsigned uid, unsigned meshUID);
		void Unload();
	public:
		std::string boneName;
		unsigned UID = 0u;
		unsigned meshUID = 0u;
		unsigned numVertexAffected = 0u;
		int* vertexId = nullptr;
		float* vertexWeight = nullptr;
		math::float4x4 offsetMatrix = math::float4x4::zero;
};

#endif // __BONE_H__
