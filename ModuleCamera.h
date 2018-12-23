#ifndef __ModuleCamera_h__
#define __ModuleCamera_h__

#include "Module.h"
#include "MathGeoLib.h"
#include "Point.h"
#include "GL/glew.h"

class ComponentCamera;

enum class Directions {
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

	math::float4x4 LookAt(math::float3& cameraPosition, math::float3& cameraFront, math::float3& cameraUp);
	void WindowResized(unsigned width, unsigned height);
	void Focus();

	ComponentCamera* sceneCamera = nullptr;
	ComponentCamera* selectedCamera = nullptr;

	int screenWidth = SCREEN_WIDTH;
	int screenHeight = SCREEN_HEIGHT;

private:

	void Move(Directions dir);
	void MouseUpdate();
	void SetVerticalFOV(float fovY);
	void SetHorizontalFOV(float fovX);
	void Orbit();
	void SetPlaneDistances(float nearDist, float farDist);
};

#endif /* __ModuleCamera_h__ */
