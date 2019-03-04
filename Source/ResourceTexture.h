#ifndef __ResourceTexture_h__
#define __ResourceTexture_h__

#include "Resource.h"

class ResourceTexture : public Resource
{
public:
	ResourceTexture(unsigned uid);
	ResourceTexture(const ResourceTexture& resource);
	virtual ~ResourceTexture();

	void Copy(const Resource& resource) override;

	bool LoadInMemory() override;
	void DeleteFromMemory() override;
	void Save(JSON_value &config) const override;
	void Load(const JSON_value &config) override;

public:
	unsigned width = 0u;
	unsigned height = 0u;
	unsigned depth = 0u;
	unsigned mips = 0u;		// Type of mipmaps (None = 0, Nearest = 1, Linear = 2)
	unsigned bytes = 0u;
	unsigned gpuID = 0u;
	unsigned format = 0u;
};

#endif __ResourceTexture_h__