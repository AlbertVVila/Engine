#include "ModuleTime.h"


ModuleTime::ModuleTime()
{
}


ModuleTime::~ModuleTime()
{
}

update_status ModuleTime::Update(float dt)
{

	Uint64 now = SDL_GetPerformanceCounter();
	dt = (double)((now - before)) / SDL_GetPerformanceFrequency();
	before = now;

	return UPDATE_CONTINUE;
}
