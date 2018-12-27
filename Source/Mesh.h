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

	//unsigned int GetMaterialIndex() const;
	void ComputeBBox();
	AABB GetBoundingBox() const;
	void DeleteBuffers();

public:
	unsigned UID = 0;
	unsigned VAO = 0;
	unsigned VBO = 0;
	unsigned EBO = 0;
	int numIndices = 0;
	std::vector<float3> vertices;
	AABB boundingBox;
};

#endif
