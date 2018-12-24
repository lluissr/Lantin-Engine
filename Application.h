#pragma once

#include<list>
#include "Globals.h"
#include "Module.h"
#include "SDL.h"

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleEditor;
class ModuleCamera;
class ModuleProgram;
class ModuleModelLoader;
class ModuleScene;
class ModuleDebugDraw;
class ModuleTime;
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
	ModuleScene* scene = nullptr;
	ModuleDebugDraw* debugDraw = nullptr;
	ModuleTime* time = nullptr;

	Timer* debugTimer = nullptr;

private:
	std::list<Module*> modules;
};

extern Application* App;
