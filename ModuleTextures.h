#pragma once
#include<list>
#include "Module.h"
#include "Globals.h"
#include "GL/glew.h"

#define LINEAR 1
#define NEAREST 0

struct SDL_Texture;

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init();
	bool CleanUp();

	void DrawGUI();

	Texture const Load(const char* path);

	int width = 0; //Make local
	int height = 0;
	int format = 0;
	int pixelDepth = 0;

	int filter_type = LINEAR;
	int wrap_mode = 0;


};