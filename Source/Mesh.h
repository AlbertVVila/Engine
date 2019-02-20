#ifndef __Mesh_h__
#define __Mesh_h__

#include <vector>
#include "Math/float3.h"
#include "Geometry/AABB.h"

//TODO: Deprecate and use ResourceMesh
class Mesh
{
public:
	Mesh();
	~Mesh();

	void SetMesh(const char* meshData, unsigned uid);
	void Draw(unsigned shaderProgram) const;
	void DrawBbox(unsigned shaderProgram, const AABB &globalBBOX) const;

	AABB GetBoundingBox() const;

	bool Intersects(const LineSegment &line, float* distance);

private:
	void ComputeBBox();
	void SetMeshBuffers(bool hasNormals, bool hasTexCoords, float* normals, float* texCoords);
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
	unsigned numIndices = 0;
	unsigned numVertices = 0;
	int* indices = nullptr;
	float* vertices = nullptr;
};

#endif
