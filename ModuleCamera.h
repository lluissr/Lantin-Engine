#ifndef __ModuleCamera_h__
#define __ModuleCamera_h__

#include "Module.h"
#include "MathGeoLib.h"

class ModuleCamera : public Module
{
public:
	ModuleCamera();
	~ModuleCamera();

	bool            Init();
	update_status   Update();
	bool            CleanUp();

private:
	Frustum frustum;
	unsigned vbo = 0;

};

#endif /* __ModuleCamera_h__ */
