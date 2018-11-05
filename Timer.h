#pragma once
#include "SDL.h"

class Timer {
public:

	Timer()
	{}

	~Timer()
	{}

	Uint32 timeStart;
	Uint64 performancetimeStart;

	void Start()
	{
		timeStart = SDL_GetTicks();
	}

	Uint32 Stop()
	{
		return SDL_GetTicks() - timeStart;
	}

	void StartPerformance()
	{
		performancetimeStart = SDL_GetPerformanceCounter();
	}

	Uint64 StopPerformance()
	{
		return SDL_GetPerformanceCounter() - performancetimeStart;
	}

};