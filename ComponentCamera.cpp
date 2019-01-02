#include "ComponentCamera.h"
#include "GameObject.h"

ComponentCamera::ComponentCamera(GameObject* parent, ComponentType type) : Component(parent, type)
{
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = math::float3(0.0f, 100.0f, -1000.0f);
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 10000.0f;
	frustum.verticalFov = math::pi / 4.0f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * ((float)screenWidth / (float)screenHeight));
}


ComponentCamera::~ComponentCamera()
{
}

math::float4x4 ComponentCamera::LookAt(math::float3& cameraPosition, math::float3& cameraFront, math::float3& cameraUp)
{
	math::float4x4 matrix;

	cameraFront.Normalize();
	math::float3 side(cameraFront.Cross(cameraUp));
	side.Normalize();
	math::float3 up(side.Cross(cameraFront));

	matrix[0][0] = side.x; matrix[0][1] = side.y; matrix[0][2] = side.z;
	matrix[1][0] = up.x; matrix[1][1] = up.y; matrix[1][2] = up.z;
	matrix[2][0] = -cameraFront.x; matrix[2][1] = -cameraFront.y; matrix[2][2] = -cameraFront.z;
	matrix[0][3] = -side.Dot(cameraPosition); matrix[1][3] = -up.Dot(cameraPosition); matrix[2][3] = cameraFront.Dot(cameraPosition);
	matrix[3][0] = 0.0f; matrix[3][1] = 0.0f; matrix[3][2] = 0.0f; matrix[3][3] = 1.0f;
	return matrix;
}

void ComponentCamera::WindowResized(unsigned width, unsigned height)
{
	screenWidth = width;
	screenHeight = height;
	SetHorizontalFOV(fovX);
	SetVerticalFOV(fovY);
}


void ComponentCamera::SetHorizontalFOV(float fovX) {
	frustum.horizontalFov = math::DegToRad(fovX);
	frustum.verticalFov = 2.0f * atanf(tanf(frustum.horizontalFov * 0.5f) * ((float)screenHeight / (float)screenWidth));
}


void ComponentCamera::SetVerticalFOV(float fovY) {
	frustum.verticalFov = math::DegToRad(fovY);
	frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov * 0.5f) * ((float)screenWidth / (float)screenHeight));
}

void ComponentCamera::CopyFromComponentCamera(const ComponentCamera& componentCamera)
{
	active = componentCamera.active;
	frustum = componentCamera.frustum;
	active = componentCamera.active;
	rSpeed = componentCamera.rSpeed;
	mSpeed = componentCamera.mSpeed;
	pitch = componentCamera.pitch;
	yaw = componentCamera.yaw;
	firstMouse = componentCamera.firstMouse;
	lastX = componentCamera.lastX;
	lastY = componentCamera.lastY;
	fovY = componentCamera.fovY;
	fovX = componentCamera.fovX;
}

void ComponentCamera::SaveJSON(Config * config)
{
	config->StartObject();

	config->AddString("uuid", uuid.c_str());
	config->AddComponentType("componentType", componentType);
	config->AddBool("isActive", active);
	
	if (myGameObject != nullptr)
	{
		config->AddString("gameObjectParent", myGameObject->uuid.c_str());
	}

	config->AddFloat("nearPlaneDistance", frustum.nearPlaneDistance);
	config->AddFloat("farPlaneDistance", frustum.farPlaneDistance);
	config->AddFloat3("position", frustum.pos);
	config->AddFloat3("front", frustum.front);
	config->AddFloat3("up", frustum.up);
	config->AddFloat("pitch", pitch);
	config->AddFloat("yaw", yaw);
	config->AddFloat("rSpeed", rSpeed);
	config->AddFloat("mSpeed", mSpeed);

	config->EndObject();
}

void ComponentCamera::LoadJSON(Config* config, rapidjson::Value& value)
{
	uuid = std::string(config->GetString("uuid", value));
	active = config->GetBool("isActive", value);

	frustum.nearPlaneDistance = config->GetFloat("nearPlaneDistance", value);
	frustum.farPlaneDistance = config->GetFloat("farPlaneDistance", value);
	frustum.pos = config->GetFloat3("position", value);
	frustum.front = config->GetFloat3("front", value);
	frustum.up = config->GetFloat3("up", value);
	pitch = config->GetFloat("pitch", value);
	yaw = config->GetFloat("yaw", value);
	rSpeed = config->GetFloat("rSpeed", value);
	mSpeed = config->GetFloat("mSpeed", value);
}

