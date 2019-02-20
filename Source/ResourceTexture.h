#ifndef __ResourceTexture_h__
#define __ResourceTexture_h__

#include "Resource.h"

struct Texture;

enum class FORMAT 
{
	COLOR_INDEX,
	RGB,
	RGBA,
	BGR,
	BGRA,
	LUMINANCE,
	UNKNOWN
};

class ResourceTexture : public Resource
{
public:
	ResourceTexture(unsigned uid);
	virtual ~ResourceTexture();

	//bool LoadInMemory() override;
	void Save(JSON_value &config) const override;
	void Load(const JSON_value &config) override;

public:
	unsigned width = 0u;
	unsigned height = 0u;
	unsigned depth = 0u;
	unsigned mips = 0u;
	unsigned bytes = 0u;
	unsigned gpuID = 0u;
	FORMAT format = FORMAT::UNKNOWN;
};

#endif __ResourceTexture_h__