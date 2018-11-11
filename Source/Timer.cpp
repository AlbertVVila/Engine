#include "Timer.h"
#include "SDL_timer.h"


Timer::Timer()
{
}


Timer::~Timer()
{
}

void Timer::Start()
{
	InitTime = SDL_GetTicks();
	timerOn = true;
}

int Timer::Read() const
{
	if (timerOn)
	{
		return SDL_GetTicks() - InitTime;
	}
	else return TotalTime;
}

int Timer::Stop()
{
	timerOn = false;
	TotalTime = SDL_GetTicks() - InitTime;
	return TotalTime;
}
