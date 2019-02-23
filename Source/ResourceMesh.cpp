#include "ResourceMesh.h"

#include "JSON.h"

ResourceMesh::ResourceMesh(unsigned uid) : Resource(uid, TYPE::MESH)
{
}

ResourceMesh::ResourceMesh(const ResourceMesh& resource) : Resource(resource)
{
	VAO = resource.VAO;
	VBO = resource.VBO;
	EBO = resource.EBO;

	boundingBox = resource.boundingBox;
	VAObox = resource.VAObox;
	VBObox = resource.VBObox;
	EBObox = resource.EBObox;

	numIndices = resource.numIndices;
	numVertices = resource.numVertices;
	indices = resource.indices;
	vertices = resource.vertices;
}

ResourceMesh::~ResourceMesh()
{
}

void ResourceMesh::Save(JSON_value &config) const
{
	Resource::Save(config);
	//TODO: Add variables to save
}

void ResourceMesh::Load(const JSON_value &config)
{
	Resource::Load(config);
	//TODO: Add variables to load
}