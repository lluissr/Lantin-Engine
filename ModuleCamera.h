#ifndef __ModuleCamera_h__
#define __ModuleCamera_h__

#include "Module.h"
#include "MathGeoLib.h"
#include "Point.h"
#include "GL/glew.h"

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
	void SetFOV(float fov);
	void SetPlaneDistances(float nearDist, float farDist);
	math::float4x4 LookAt(math::float3& cameraPosition, math::float3& cameraFront, math::float3& cameraUp);
	void Move(Directions dir);
	void Rotate(Directions dir);
	void MouseUpdate(const iPoint& mousePosition);
	void Zoom(const iPoint& mousePosition);

	Frustum frustum;
	math::float3& cameraPosition = math::float3(0.0f, 1.0f, 10.0f);
	math::float3& cameraFront = math::float3(0.0f, 0.0f, -1.0f);
	math::float3& cameraUp = math::float3(0.0f, 1.0f, 0.0f);

	float mSpeed = 0.5f;
	float rSpeed = 1.0f;

	float pitch = 0;
	float yaw = 0;

	bool firstMouse = true;
	int lastX = 0;
	int lastY = 0;

	float fov = 45.0f;

	void RefenceGround();
	void ReferenceAxis();

};

#endif /* __ModuleCamera_h__ */
