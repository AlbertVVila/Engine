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

	bool LoadInMemory() override { return false; };
	void Save(JSON_value &config) const override;
	void Load(const JSON_value &config) override;

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
};

#endif __ResourceMesh_h__