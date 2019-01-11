#ifndef __Mesh_h__
#define __Mesh_h__

#include <vector>
#include "Math/float3.h"
#include "Geometry/AABB.h"

class Mesh
{
public:
	Mesh();
	~Mesh();

	void SetMesh(const char* meshData, unsigned uid);
	void Draw(unsigned int shaderProgram) const;

	void ComputeBBox();
	AABB GetBoundingBox() const;

	bool Intersects(const LineSegment &line, float* distance);

public:
	unsigned UID = 0;
	unsigned VAO = 0;
	unsigned VBO = 0;
	unsigned EBO = 0;

	unsigned numIndices = 0;
	unsigned numVertices = 0;

	int* indices = nullptr;
	float* vertices = nullptr;
	AABB boundingBox;
};

#endif
