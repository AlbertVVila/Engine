#ifndef __ResourceModel_h__
#define __ResourceModel_h__

#include "Resource.h"

#include <vector>

class ResourceMesh;

class ResourceModel :
	public Resource
{
public:
	ResourceModel(unsigned uid);
	ResourceModel(const ResourceModel& resource);
	~ResourceModel();

	bool LoadInMemory() override;		// Will load in memory all ResourceMeshes stored
	void DeleteFromMemory() override;	// Will delete all ResourceMeshes stored in memory

	// File in asset specific
	void Rename(const char* newName) override;
	void Delete() override;

	void SaveMetafile(const char* file) const override;
	void LoadConfigFromMeta() override;
	bool CheckImportedMeshes();

	void AddMesh(ResourceMesh* mesh);
	void DrawImportConfiguration() override;

public:
	unsigned numMeshes = 0u;
	std::vector<ResourceMesh*> meshList;
};

#endif __ResourceModel_h__