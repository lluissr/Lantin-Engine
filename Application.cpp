#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleProgram.h"
#include "ModuleModelLoader.h"
#include "ModuleRenderExercise.h"

using namespace std;

Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules.push_back(window = new ModuleWindow());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(textures = new ModuleTextures());
	modules.push_back(input = new ModuleInput());
    modules.push_back(camera = new ModuleCamera());
	modules.push_back(program = new ModuleProgram());
	modules.push_back(modelLoader = new ModuleModelLoader());
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(exercise = new ModuleRenderExercise());

	FPSInit();

}

Application::~Application()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }
}

bool Application::Init()
{
	bool ret = true;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	return ret;
}

update_status Application::Update()
{
	FPSCalculation();

	update_status ret = UPDATE_CONTINUE;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for(list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}

void Application::FPSInit() 
{
	memset(frametimes, 0, sizeof(frametimes));
	framecount = 0;
	fps = 0;
	frametimelast = SDL_GetTicks();
}

void Application::FPSCalculation() 
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
