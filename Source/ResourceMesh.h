#ifndef __ResourceMesh_h__
#define __ResourceMesh_h__

#include "Resource.h"
#include "Geometry/AABB.h"

class ResourceMesh : public Resource
{
public:
	ResourceMesh(unsigned uid);
	ResourceMesh(const ResourceMesh& resource);
	virtual ~ResourceMesh();

	bool LoadInMemory() override;
	void DeleteFromMemory() override;
	void Save(JSON_value &config) const override;
	void Load(const JSON_value &config) override;

	// Mesh specific
	void Draw(unsigned shaderProgram) const;
	void DrawBbox(unsigned shaderProgram, const AABB &globalBBOX) const;
	AABB GetBoundingBox() const;
	bool Intersects(const LineSegment &line, float* distance);

private:
	void ComputeBBox();
	void SetMesh(const char* meshData);
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
	unsigned numIndices = 0;
	unsigned numVertices = 0;
	int* indices = nullptr;
	float* vertices = nullptr;

	// New added on refactor
	float* normals = nullptr;
	float* texCoords = nullptr;
};

#endif __ResourceMesh_h__