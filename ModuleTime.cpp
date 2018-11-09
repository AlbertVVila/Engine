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

	float currentFrame = SDL_GetTicks();
	deltaTime = (currentFrame - lastFrame)*0.001f;
	lastFrame = currentFrame;

	return UPDATE_CONTINUE;
}
