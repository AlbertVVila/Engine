#pragma once
#include<list>
#include "Module.h"
#include "Globals.h"
#include "GL/glew.h"
struct SDL_Texture;

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init();
	bool CleanUp();

	GLuint const Load(const char* path);

	int width = 0;
	int height = 0;
	int format = 0;
	int pixelDepth = 0;

	bool mipmap = false;
	bool magfilter = true;
	bool minfilter = true;
	bool anisotropic_filter = false;
	int wrap_mode = 0;


};