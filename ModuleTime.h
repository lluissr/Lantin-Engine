#ifndef __ModuleTime_h__
#define __ModuleTime_h__
#include "Module.h"
#include "Timer.h"

enum class State { RUN, PAUSE, STOP };

class ModuleTime :public Module
{
public:
	ModuleTime();
	~ModuleTime();
	bool Init() override;

	update_status Update() override;
	bool CleanUp() override;
	void Start();
	void Pause(bool pause);
	void Stop();

	float gameTime = 0.0f;
	float gameDeltaTime = 0.0f;

	float realTime = 0.0f;
	float realDeltaTime = 0.0f;
	unsigned realFrameCount = 0u;
	unsigned frameCount = 0u;
	unsigned totalFrames = 0u;

	State gameState = State::STOP;

	Timer frameTimer;
	Timer fpsTimer;


	Uint32 frametimes[FRAME_VALUES];
	Uint32 frametimelast;
	Uint32 framecount;
	float fps;

	void FPSInit();
	void FPSCalculation();

};

#endif
