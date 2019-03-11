#ifndef __Mesh_h__
#define __Mesh_h__

#include <vector>
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/AABB.h"

#define MAX_WEIGHTS_PER_BONE 4

class GameObject;
class ComponentRenderer;

class Mesh
{
public:
	Mesh();
	~Mesh();

	void SetMesh(const char* meshData, unsigned uid);
	void Draw(unsigned shaderProgram) const;
	void DrawBbox(unsigned shaderProgram, const AABB &globalBBOX) const;
	void LinkBones(const ComponentRenderer* renderer);
	AABB GetBoundingBox() const;

	bool Intersects(const LineSegment &line, float* distance);

private:

	struct BindBone
	{
		math::float4x4 transform; //Transforms from mesh space to bone space
		std::string name;
		GameObject* go = nullptr;
	};

	struct BoneVertex
	{
		int boneID[MAX_WEIGHTS_PER_BONE] = { 0, 0, 0, 0 };
	};

	struct BoneWeight
	{
		float weight[MAX_WEIGHTS_PER_BONE] = { 0.f, 0.f, 0.f, 0.f };
	};
	
private:
	void ComputeBBox();
	void ProcessVertexTangent(const float vIndex1, const float vIndex2, const float vIndex3);
	void CalculateTangents();
	void SetMeshBuffers();
	void SetBboxBuffers();

private:
	unsigned VAO = 0;
	unsigned VBO = 0;
	unsigned EBO = 0;

	AABB boundingBox;
	unsigned VAObox = 0;
	unsigned VBObox = 0;
	unsigned EBObox = 0;

public:
	unsigned UID = 0;

	std::vector<math::float3> meshVertices;
	std::vector<math::float3> meshNormals;
	std::vector<math::float3> meshTangents;
	std::vector<float> meshTexCoords;
	std::vector<unsigned> meshIndices;

	std::vector<BoneVertex> bindBoneVertexAttaches;
	std::vector<BoneWeight> bindWeightVertexAttaches;
	std::vector<BindBone> bindBones;
};

#endif
