#include "ModuleTime.h"



ModuleTime::ModuleTime()
{
	FPSInit();
}


ModuleTime::~ModuleTime()
{
}

bool ModuleTime::Init() {
	gameDeltaTime = 0.0f;
	frameTimer.Start();
	fpsTimer.Start();
	return true;
}


update_status ModuleTime::Update() {
	FPSCalculation();
	++frameCount;
	++realFrameCount;

	realDeltaTime = frameTimer.ReadSeconds();
	realTime += realDeltaTime;


	if (gameState == State::RUN) {
		++totalFrames;
		gameDeltaTime = frameTimer.ReadSeconds();
		gameTime += gameDeltaTime;
	}

	frameTimer.Reset();

	if (fpsTimer.ReadSeconds() >= 1.0f) {
		frameCount = 0u;
		fpsTimer.Reset();
	}

	return UPDATE_CONTINUE;
}


bool ModuleTime::CleanUp() {
	frameTimer.Stop();
	fpsTimer.Stop();
	return true;
}

void ModuleTime::Start() {
	gameState = State::RUN;
}

void ModuleTime::Pause(bool pause) {
	if (pause) {
		gameState = State::PAUSE;
		gameDeltaTime = 0.0f;
	}
	else {
		gameState = State::RUN;
	}
}

void ModuleTime::Stop() {
	gameState = State::STOP;
	gameDeltaTime = 0.0f;
	gameTime = 0.0f;
	totalFrames = 0u;
}

void ModuleTime::FPSInit()
{
	memset(frametimes, 0, sizeof(frametimes));
	framecount = 0;
	fps = 0;
	frametimelast = SDL_GetTicks();
}

void ModuleTime::FPSCalculation()
{
	Uint32 frametimesindex;
	Uint32 getticks;
	Uint32 count;
	Uint32 i;

	frametimesindex = framecount % FRAME_VALUES;

	getticks = SDL_GetTicks();

	frametimes[frametimesindex] = getticks - frametimelast;

	frametimelast = getticks;
	framecount++;

	if (framecount < FRAME_VALUES)
	{
		count = framecount;
	}
	else
	{
		count = FRAME_VALUES;
	}

	fps = 0;
	for (i = 0; i < count; i++)
	{
		fps += frametimes[i];
	}

	fps /= count;
	fps = 1000.f / fps;
}