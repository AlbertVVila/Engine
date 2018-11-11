#ifndef __ModuleTextures_h__
#define __ModuleTextures_h__

#include "Module.h"

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
	Texture const Load(const char* path) const;

public:
	int filter_type = LINEAR;
	int wrap_mode = 0;
};

#endif /* __ModuleTextures_h__ */