#ifndef __ComponentCamera_h__
#define __ComponentCamera_h__

#include "Component.h"
#include "MathGeoLib.h"
#include "Globals.h"

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject* parent, ComponentType type);
	~ComponentCamera();

	math::float4x4 LookAt(math::float3& cameraPosition, math::float3& cameraFront, math::float3& cameraUp);
	void WindowResized(unsigned width, unsigned height);
	void SetVerticalFOV(float fovY);
	void SetHorizontalFOV(float fovX);
	void CopyFromComponentCamera(const ComponentCamera& componentCamera);

	void SaveJSON(Config* config) override;

	bool showFrustum = true;

	math::Frustum frustum;
	int screenWidth = SCREEN_WIDTH;
	int screenHeight = SCREEN_HEIGHT;

	float mSpeed = 50.0f;
	float rSpeed = 1.0f;

	float pitch = 0;
	float yaw = 180;

	bool firstMouse = true;
	int lastX = 0;
	int lastY = 0;

	float fovY = 45.0f;
	float fovX = 45.0f;
};

#endif;
