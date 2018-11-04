#ifndef __ModuleTextures_h__
#define __ModuleTextures_h__

#include<list>
#include "Module.h"
#include "Globals.h"
#include "GL/glew.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"

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

	ILinfo lastImageInfo;
};

#endif