#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"

class Application;
struct SDL_Window; 

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	virtual ~ModuleWindow();


	bool Init() override;
	bool CleanUp() override;

	void Resize(int width, int height);
	void DrawGUI();

public:
	//The window we'll be rendering to
	SDL_Window* window = NULL;
	
	int width = SCREEN_WIDTH;
	int height = SCREEN_HEIGHT;

private:

	float brightness = 1.0f;
	bool fullscreen = false;
	bool resizable = true;
	bool borderless = false;
	bool fullscreen_desktop = false;

	int previouswidth = SCREEN_WIDTH;
	int previousheight = SCREEN_HEIGHT;

};

#endif // __ModuleWindow_H__