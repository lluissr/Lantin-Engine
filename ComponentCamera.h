#ifndef __ComponentCamera_h__
#define __ComponentCamera_h__

#include "Component.h"
#include "MathGeoLib.h"
#include "Globals.h"

class ComponentCamera :	public Component
{
public:
	ComponentCamera(GameObject* parent, ComponentType type);
	~ComponentCamera();

	math::Frustum frustum;
	int screenWidth = SCREEN_WIDTH;
	int screenHeight = SCREEN_HEIGHT;

	float mSpeed = 0.5f;
	float rSpeed = 1.0f;

	float pitch = 0;
	float yaw = 0;

	bool firstMouse = true;
	int lastX = 0;
	int lastY = 0;

	float fovY = 45.0f;
	float fovX = 45.0f;
};

#endif;
