#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL_video.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	// Destructor
	virtual ~ModuleWindow();

	// Called before quitting
	bool Init();

	// Called before quitting
	bool CleanUp();

	void Resize(int width, int height);

	void DrawGUI();

public:
	//The window we'll be rendering to
	SDL_Window* window = NULL;
	
	float brightness = 1.0f;
	bool fullscreen = false;
	bool resizable = true;
	bool borderless = false;
	bool fullscreen_desktop = false;

	int width = SCREEN_WIDTH;
	int height = SCREEN_HEIGHT;
	int previouswidth = SCREEN_WIDTH;
	int previousheight = SCREEN_HEIGHT;

};

#endif // __ModuleWindow_H__