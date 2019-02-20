#include "ResourceMesh.h"

#include "JSON.h"

ResourceMesh::ResourceMesh(unsigned uid) : Resource(uid, TYPE::MESH)
{
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