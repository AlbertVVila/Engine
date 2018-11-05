#include "Timer.h"
#include "SDL.h"


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

int Timer::Read()
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

void Timer::StartPrecise()
{
	timerPreciseOn = true;
	startPrecise = SDL_GetPerformanceCounter();
}

double Timer::ReadPrecise()
{
	if (timerPreciseOn)
	{
		return ((double)(SDL_GetPerformanceCounter() - startPrecise) * 1000) / (double)SDL_GetPerformanceFrequency();
	}
	else return TotalPrecise;
}

double Timer::StopPrecise()
{
	timerPreciseOn = false;
	TotalPrecise = ((double)(SDL_GetPerformanceCounter() - startPrecise) * 1000) / (double)SDL_GetPerformanceFrequency();
	return TotalPrecise;
}
