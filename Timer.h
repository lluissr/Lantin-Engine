#ifndef __Timer_h__
#define __Timer_h__

#include "SDL.h"

class Timer
{
public:
	bool running = false;

private:
	float time = 0.0f;
	Uint32 startTicks = 0;
	Uint32 skippedTime = 0;
	Uint64 performancetimeStart;

public:
	Timer() {};
	~Timer() {};

	inline void Start() {
		startTicks = SDL_GetTicks();
		running = true;
	}

	inline float Read() {
		if (running)
			time = (float)(SDL_GetTicks() - startTicks + skippedTime);
		return time;
	}

	inline float ReadSeconds() {
		if (running)
			time = (SDL_GetTicks() - startTicks + skippedTime) / 1000.0f;
		return time;
	}

	inline void Pause() {
		skippedTime += (SDL_GetTicks() - startTicks);
		running = false;
	}

	inline void Stop() {
		running = false;
	}

	inline void Reset() {
		startTicks = SDL_GetTicks();
		skippedTime = 0;
	}

	inline void StartPerformance()
	{
		performancetimeStart = SDL_GetPerformanceCounter();
	}

	inline Uint64 StopPerformance()
	{
		return SDL_GetPerformanceCounter() - performancetimeStart;
	}
};

#endif