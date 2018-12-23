#include "ComponentCamera.h"



ComponentCamera::ComponentCamera(GameObject* parent, ComponentType type) : Component(parent, type)
{
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = math::float3(0.0f, 1.0f, -10.0f);
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 1000.0f;
	frustum.verticalFov = math::pi / 4.0f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * ((float)screenWidth / (float)screenHeight));
}


ComponentCamera::~ComponentCamera()
{
}
