#ifndef __ModuleRenderExercise_h__
#define __ModuleRenderExercise_h__

#include "Module.h"
#include "MathGeoLib.h"
#include "GL/glew.h"

class ModuleRenderExercise : public Module
{
public:
    ModuleRenderExercise();
    ~ModuleRenderExercise();

	bool            Init    ();
	update_status   Update  ();
	bool            CleanUp ();


private:
	GLuint texture;
	unsigned vbo = 0;
	void RefenceGround();
	void ReferenceAxis();

};

#endif
