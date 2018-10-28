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
	update_status   PreUpdate();
	update_status   Update();
	bool            CleanUp();
	void SetAspectRatio(float aspect_ratio);
	math::float4x4 GetProjectionMatrix();
	math::float4x4	GetViewMatrix();
	void SetFOV(float fov);
	void SetPlaneDistances(float nearDist, float farDist);
	
	/*
	Position()
	Orientation()
	LookAt(x,y,z)
	*/

	Frustum frustum;

private:
	
	unsigned vbo = 0;
	void RefenceGround();
	float dx = 10;
	float dy = 10;

};

#endif /* __ModuleCamera_h__ */
