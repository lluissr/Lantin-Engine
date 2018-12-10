#ifndef __ModuleProgram_h__
#define __ModuleProgram_h__

#include "Module.h"
#include "GL/glew.h"
#include "SDL.h"
#include <assert.h>

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
	GLuint CreateProgram(const char* vertexShader, const char* fragmentShader) const ;
	
	GLuint program;
	GLuint colorProgram;
	GLuint flatProgram;
	GLuint gouraudProgram;
	GLuint phongProgram;
	GLuint blinnProgram;
};

#endif

