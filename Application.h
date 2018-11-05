#pragma once

#include<list>
#include "Globals.h"
#include "Module.h"
#include "SDL.h"

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleRenderExercise;
class ModuleEditor;
class ModuleCamera;
class ModuleProgram;
class ModuleModelLoader;
class Timer;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

public:
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleInput* input = nullptr;
	ModuleCamera* camera = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleProgram* program = nullptr;
	ModuleModelLoader* modelLoader = nullptr;
	ModuleRenderExercise* exercise = nullptr;
	Timer* t = nullptr;
	float fps;

private:
	std::list<Module*> modules;

	Uint32 frametimes[FRAME_VALUES];
	Uint32 frametimelast;
	Uint32 framecount;

	void FPSInit();
	void FPSCalculation();

};

extern Application* App;
