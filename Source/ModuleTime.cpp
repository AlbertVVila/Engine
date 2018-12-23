#include "ModuleTime.h"


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

update_status ModuleTime::Update() //TODO: not getting time correctly between frames -> change location?
{

	Uint64 now = SDL_GetPerformanceCounter();
	dt = (double)((now - before)) / SDL_GetPerformanceFrequency();
	before = now;

	return UPDATE_CONTINUE;
}
