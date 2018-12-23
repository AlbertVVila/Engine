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
	unsigned id;
	unsigned width;
	unsigned height;
	std::string file;
	Texture(unsigned id, unsigned width, unsigned height, std::string file) : id(id), width(width), height(height), file(file)
	{}
};

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init();
	bool CleanUp();
	void DrawGUI();
	Texture * Load(const char* path) const;
	unsigned LoadCubeMap(const std::string faces[]);

	void ImportImage(const char * file, const char* folder);

public:
	int filter_type = LINEAR;
};

#endif /* __ModuleTextures_h__ */