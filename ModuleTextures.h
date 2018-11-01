#pragma once
#include<list>
#include "Module.h"
#include "Globals.h"
#include "GL/glew.h"

struct SDL_Texture;

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init();
	bool CleanUp();

	GLuint Load(const char* path);
	void Unload(unsigned id);
};