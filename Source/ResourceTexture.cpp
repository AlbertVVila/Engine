#include "ResourceTexture.h"

#include "JSON.h"

ResourceTexture::ResourceTexture(unsigned uid) : Resource(uid, TYPE::TEXTURE)
{
}


ResourceTexture::~ResourceTexture()
{
}

void ResourceTexture::Save(JSON_value &config) const
{
	Resource::Save(config);

	config.AddUint("Width", width);
	config.AddUint("Height", height);
	config.AddUint("Depth", depth);
	config.AddUint("Mips", mips);
	config.AddUint("Bytes", bytes);
	config.AddUint("GpuID", gpuID);

	config.AddUint("Format", (unsigned)format);
}

void ResourceTexture::Load(const JSON_value &config)
{
	Resource::Load(config);

	width = config.GetUint("Width");
	height = config.GetUint("Height");
	depth = config.GetUint("Depth");
	mips = config.GetUint("Mips");
	bytes = config.GetUint("Bytes");
	gpuID = config.GetUint("GpuID");

	format = (FORMAT)config.GetUint("Format");
}