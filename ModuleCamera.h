#ifndef __ModuleCamera_h__
#define __ModuleCamera_h__

#include "Module.h"
#include "MathGeoLib.h"


enum Directions {
	UP,
	DOWN,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

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
	math::float4x4 LookAt(math::float3& target, math::float3& eye, math::float3& up);
	void Move(Directions dir);


	Frustum frustum;
	math::float3& target = math::float3(0.0f, 0.0f, 0.0f);
	math::float3& eye = math::float3(0.0f, 2.0f, 5.0f);
	math::float3& up = math::float3(0.0f, 1.0f, 0.0f);
	math::float3 forward;
	math::float3 side;
	math::float3 upLU;

	float speed = 0.5f;
	
private:
	
	unsigned vbo = 0;
	void RefenceGround();
	void ReferenceAxis();
};

#endif /* __ModuleCamera_h__ */
