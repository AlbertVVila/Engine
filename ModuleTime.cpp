#include "ModuleTime.h"
#include "SDL.h"


ModuleTime::ModuleTime()
{
}


ModuleTime::~ModuleTime()
{
}

bool ModuleTime::Init()
{
	return true;
}

update_status ModuleTime::PreUpdate()
{

	long now = SDL_GetPerformanceCounter();
	deltaTime = (double)((now - before)) / SDL_GetPerformanceFrequency();
	before = now;

	return UPDATE_CONTINUE;
}
