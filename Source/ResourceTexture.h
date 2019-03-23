#ifndef __ResourceTexture_h__
#define __ResourceTexture_h__

#include "Resource.h"

enum class IMAGE_TYPE {TEXTURE, CUBEMAP};

enum class DXT
{
	DXT1 = 0x0706, // Working
	DXT2 = 0x0707, // Error
	DXT3 = 0x0708, // Working
	DXT4 = 0x0709, // Error
	DXT5 = 0x070A, // Working
	DXT_NO_COMP = 0x070B,// Error
};

class ResourceTexture : public Resource
{
public:
	ResourceTexture(unsigned uid);
	ResourceTexture(const ResourceTexture& resource);
	virtual ~ResourceTexture();

	void Copy(const Resource& resource) override;

	bool LoadInMemory() override;
	void DeleteFromMemory() override;
	void SaveMetafile(const char* file) const override;
	void Load(const JSON_value& config) override;

	inline IMAGE_TYPE GetImageType() { return imageType; };
	void SetImageType(IMAGE_TYPE type);
	inline unsigned GetCubemapIndex() { return cubemapIndex; };
	inline void SetCubemapIndex(unsigned index) { cubemapIndex = index; };

	void SetImportConfiguration() override;

private:
	bool LoadTexture();
	bool LoadCubemap();

public:
	unsigned width = 0u;
	unsigned height = 0u;
	unsigned depth = 0u;
	unsigned mips = 0u;			// Type of mipmaps (None = 0, Nearest = 1, Linear = 2)
	unsigned bytes = 0u;
	unsigned gpuID = 0u;
	unsigned format = 0u;

	// Compression type
	DXT dxtFormat = DXT::DXT5;
	int compression = 4;

private:
	IMAGE_TYPE imageType = IMAGE_TYPE::TEXTURE;
	unsigned cubemapIndex = 0u;	// If the image is a cubemap this variable will tell the index ( 0=right, 1=left, 2=top, 3=bottom, 4=front, 5=back)
};

#endif __ResourceTexture_h__