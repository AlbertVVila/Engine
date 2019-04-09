#ifndef __ResourceScene_h__
#define __ResourceScene_h__

#include "Resource.h"

class GameObject;

class ResourceScene :
	public Resource
{
public:
	ResourceScene(unsigned uid);
	ResourceScene(const ResourceScene& resource);
	virtual ~ResourceScene();

	void SaveMetafile(const char* file) const override;

	// File in Assets especific
	void Rename(const char* newName) override;
	void Delete() override;

	// Scene especific
	void Save(const GameObject &rootGO);
	bool Load();
};

#endif