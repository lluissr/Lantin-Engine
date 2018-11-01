#ifndef __ModuleProgram_h__
#define __ModuleProgram_h__

#include "Module.h"
#include "GL/glew.h"
#include "SDL.h"

class ModuleProgram : public Module
{
public:
	ModuleProgram();
	~ModuleProgram();

	bool            Init();
	update_status   PreUpdate();
	update_status   Update();
	bool            CleanUp();

	char* ReadShader(const char* path) const;
	
	GLuint program;
};

#endif

