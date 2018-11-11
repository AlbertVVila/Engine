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

update_status ModuleTime::PreUpdate()
{

	Uint64 now = SDL_GetPerformanceCounter();
	deltaTime = (double)((now - before)) / SDL_GetPerformanceFrequency();
	before = now;

	return UPDATE_CONTINUE;
}
