#ifndef __ModuleTextures_h__
#define __ModuleTextures_h__

#include "Module.h"
#include <vector>
#include <string>

#define LINEAR 1
#define NEAREST 0
#define NUMFACES 6

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

	void DrawGUI();
	Texture * GetTexture(const char* path) const;
	unsigned LoadCubeMap(const std::string faces[]) const;
	void ImportImage(const char * file, const char* folder) const;

public:
	int filter_type = LINEAR;
};

#endif /* __ModuleTextures_h__ */