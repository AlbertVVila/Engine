#ifndef __ResourceAudio_h__
#define __ResourceAudio_h__

#include "Resource.h"
class ResourceAudio :
	public Resource
{
public:
	ResourceAudio(unsigned uid);
	ResourceAudio(const ResourceAudio& resource);
	~ResourceAudio();

	bool LoadInMemory() override;
	void DeleteFromMemory() override;
	void SaveMetafile(const char* file) const override;
	void LoadConfigFromMeta() override;
};

#endif __ResourceAudio_h__