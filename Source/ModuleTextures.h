#ifndef __ModuleTextures_h__
#define __ModuleTextures_h__

#include "Module.h"
#include <vector>
#include <string>

#define NUMFACES 6

class ResourceTexture;

enum class FILTERTYPE
{
	LINEAR,
	NEAREST,
	NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_LINEAR
};

// TODO: Deprecate and use ResourceTexture instead
struct Texture
{
	unsigned id = 0;
	unsigned width = 0;
	unsigned height = 0;
	std::string file;
	Texture(unsigned id, unsigned width, unsigned height, std::string file) : id(id), width(width), height(height), file(file)
	{}

	Texture(std::string file) : file(file)
	{}

	~Texture();
};

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init(JSON * config) override;
	bool CleanUp() override;
	void SaveConfig(JSON * config) override;

	void DrawGUI() override;
	ResourceTexture * GetTexture(const char* path) const;
	unsigned LoadCubeMap(const std::string faces[]) const;
	bool ImportImage(const char* file, const char* folder, std::string& exportedFile, ResourceTexture* resource) const;

public:
	FILTERTYPE filter_type = FILTERTYPE::LINEAR;
};

#endif /* __ModuleTextures_h__ */