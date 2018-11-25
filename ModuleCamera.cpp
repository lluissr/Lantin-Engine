#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleProgram.h"
#include "ModuleModelLoader.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "GL/glew.h"
#include "SDL.h"
#include "MathGeoLib.h"

ModuleCamera::ModuleCamera()
{

}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.pos = math::float3(0.0f, 1.0f, 10.0f);
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 1000.0f;
	frustum.verticalFov = math::pi / 4.0f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * ((float)screenWidth / (float)screenHeight));

	return true;
}

update_status ModuleCamera::PreUpdate()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
		{
			Move(Directions::UP);
		}
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
		{
			Move(Directions::DOWN);
		}
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			Move(Directions::FORWARD);
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			Move(Directions::BACKWARD);
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			Move(Directions::LEFT);
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			Move(Directions::RIGHT);
		}
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN)
		{
			mSpeed = mSpeed * 2;
			rSpeed = rSpeed * 2;
		}
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP)
		{
			mSpeed = mSpeed / 2;
			rSpeed = rSpeed / 2;
		}

		MouseUpdate();
	}
	
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_UP)
	{
		firstMouse = true;
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_X1) == KEY_DOWN) 
	{
		fovX -= 1;
		SetHorizontalFOV(fovX);
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_X2) == KEY_DOWN) 
	{
		fovX += 1;
		SetHorizontalFOV(fovX);
	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		Focus();		
	}

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT))
	{
		Orbit();
	}


	return UPDATE_CONTINUE;
}

update_status ModuleCamera::Update()
{
	return UPDATE_CONTINUE;
}

void ModuleCamera::Orbit()
{
	
}

void ModuleCamera::Focus()
{
	math::AABB boundingBox = math::AABB(App->modelLoader->minPoint, App->modelLoader->maxPoint);
	math::float3 center = boundingBox.FaceCenterPoint(5);
	
	//Reset all variables (position, front, up, fov, pitch, yaw, firstmouse)
	frustum.pos = cameraPosition = math::float3(center.x, center.y, 0.0f);
	frustum.front = cameraFront = math::float3(0.0f, 0.0f, -1.0f);
	frustum.up = cameraUp = math::float3(0.0f, 1.0f, 0.0f);
	frustum.verticalFov = math::pi / 4.0f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * ((float)screenWidth / (float)screenHeight));
	fovY = 45.0f;
	fovX = 45.0f;
	pitch = 0;
	yaw = 0;
	firstMouse = true;

	//Add distance still we can see all the bounding box
	while (!App->camera->frustum.Contains(boundingBox))
	{
		App->camera->cameraPosition.z = App->camera->frustum.pos.z += 10;
	}
}


bool ModuleCamera::CleanUp()
{
	return true;
}


math::float4x4 ModuleCamera::LookAt(math::float3& cameraPosition, math::float3& cameraFront, math::float3& cameraUp)
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


void ModuleCamera::Move(Directions dir)
{
	switch (dir) {
	case Directions::UP:
		frustum.pos = cameraPosition += cameraUp.Normalized() * mSpeed;
		break;
	case Directions::DOWN:
		frustum.pos = cameraPosition -= cameraUp.Normalized() * mSpeed;
		break;
	case Directions::FORWARD:
		frustum.pos = cameraPosition += cameraFront.Normalized() * mSpeed;
		break;
	case Directions::BACKWARD:
		frustum.pos = cameraPosition -= cameraFront.Normalized() * mSpeed;
		break;
	case Directions::LEFT:
		frustum.pos = cameraPosition += cameraUp.Cross(cameraFront).Normalized() * mSpeed;
		break;
	case Directions::RIGHT:
		frustum.pos = cameraPosition -= cameraUp.Cross(cameraFront).Normalized() * mSpeed;
		break;
	}
}


void ModuleCamera::MouseUpdate()
{
	iPoint mousePosition = App->input->GetMousePosition();

	if (firstMouse) {
		lastX = mousePosition.x;
		lastY = mousePosition.y;
		firstMouse = false;
	}

	float xoffset = (float)mousePosition.x - (float)lastX;
	float yoffset = (float)lastY - (float)mousePosition.y;
	lastX = mousePosition.x;
	lastY = mousePosition.y;

	xoffset *= 0.5f;
	yoffset *= 0.5f;

	yaw += xoffset;
	pitch += yoffset;

	pitch = math::Clamp(pitch, -89.0f, 89.0f);

	math::float3 front;

	front.x = SDL_sinf(math::DegToRad(yaw)) * SDL_cosf(math::DegToRad(pitch));
	front.y = SDL_sinf(math::DegToRad(pitch));
	front.z = -SDL_cosf(math::DegToRad(yaw)) * SDL_cosf(math::DegToRad(pitch));
	frustum.front = cameraFront = front.Normalized();
}

void ModuleCamera::SetPlaneDistances(float nearDist, float farDist)
{
	if (nearDist > 0.0f && nearDist < frustum.farPlaneDistance)
	{
		frustum.nearPlaneDistance = nearDist;
	}

	if (farDist > 0.0f && farDist > frustum.nearPlaneDistance)
	{
		frustum.farPlaneDistance = farDist;
	}
}


void ModuleCamera::WindowResized(unsigned width, unsigned height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
	SetHorizontalFOV(fovX);
	SetVerticalFOV(fovY);
}


void ModuleCamera::SetHorizontalFOV(float fovX) {
	frustum.horizontalFov = math::DegToRad(fovX);
	frustum.verticalFov = 2.0f * atanf(tanf(frustum.horizontalFov * 0.5f) * ((float)screenHeight / (float)screenWidth));
}


void ModuleCamera::SetVerticalFOV(float fovY) {
	frustum.verticalFov = math::DegToRad(fovY);
	frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov * 0.5f) * ((float)screenWidth / (float)screenHeight));
}


void ModuleCamera::DrawImGui()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		float front[3] = { cameraFront.x, cameraFront.y, cameraFront.z };
		float up[3] = { cameraUp.x, cameraUp.y, cameraUp.z };
		float position[3] = { cameraPosition.x, cameraPosition.y, cameraPosition.z };

		ImGui::InputFloat3("Front", front);
		ImGui::InputFloat3("Up", up);
		ImGui::InputFloat3("Position", position);
		ImGui::InputFloat("Movement Speed", &mSpeed);
		ImGui::InputFloat("Rotation Speed", &rSpeed);
		ImGui::InputFloat("Pitch", &pitch);
		ImGui::InputFloat("Yaw", &yaw);
		ImGui::InputFloat("Near Plane", &frustum.nearPlaneDistance);
		ImGui::InputFloat("Far Plane", &frustum.farPlaneDistance);
	}
}


