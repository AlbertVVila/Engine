#ifndef __ModuleTextures_h__
#define __ModuleTextures_h__

#include "Module.h"
#include <vector>
#include <string>

#define LINEAR 1
#define NEAREST 0

struct Texture
{
	int id;
	int width;
	int height;
	Texture(int id, int width, int height) : id(id), width(width), height(height)
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
	unsigned int LoadCubeMap(const std::vector<std::string> &faces);

	void ImportImage(const char * file, const char* folder);

public:
	int filter_type = LINEAR;
};

#endif /* __ModuleTextures_h__ */