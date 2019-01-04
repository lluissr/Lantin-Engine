#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleModelLoader.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleRender.h"

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
	sceneCamera = new ComponentCamera(nullptr, ComponentType::CAMERA);
	selectedCamera = sceneCamera;
	sceneCamera->frustum.pos = math::float3(0.0f, 100.0f, 1000.0f);
	sceneCamera->frustum.front = -float3::unitZ;
	sceneCamera->frustum.farPlaneDistance = 100000.0f;
	sceneCamera->yaw = 0;

	return true;
}

update_status ModuleCamera::PreUpdate()
{
	if (selectedCamera != nullptr && selectedCamera->myGameObject != nullptr && !selectedCamera->myGameObject->isActive)
	{
		return UPDATE_CONTINUE;
	}

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
			selectedCamera->mSpeed = selectedCamera->mSpeed * 2;
			selectedCamera->rSpeed = selectedCamera->rSpeed * 2;
		}
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP)
		{
			selectedCamera->mSpeed = selectedCamera->mSpeed / 2;
			selectedCamera->rSpeed = selectedCamera->rSpeed / 2;
		}

		MouseUpdate();
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_UP)
	{
		if (selectedCamera != nullptr)
			selectedCamera->firstMouse = true;
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_X1) == KEY_DOWN)
	{
		selectedCamera->fovX -= 1;
		//SetHorizontalFOV(selectedCamera->fovX);
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_X2) == KEY_DOWN)
	{
		selectedCamera->fovX += 1;
		//SetHorizontalFOV(selectedCamera->fovX);
	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		Focus();
	}

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT))
	{
		Orbit();
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (App->editor->overEditorViewport)
		{
			iPoint mousePosition = App->input->GetMousePosition();

			float normalizedX = -(1.0f - (float(mousePosition.x - App->editor->editorViewportX) * 2.0f) / sceneCamera->screenWidth);
			float normalizedY = 1.0f - (float(mousePosition.y - App->editor->editorViewportY) * 2.0f) / sceneCamera->screenHeight;

			pickingLine = sceneCamera->frustum.UnProjectLineSegment(normalizedX, normalizedY);

			objects.clear();
			App->scene->quadTree.CollectIntersections(objects, pickingLine);

			for (std::vector<ComponentMesh*>::iterator it =  App->renderer->meshes.begin(); it != App->renderer->meshes.end(); ++it)
			{
				if (!(*it)->myGameObject->isStatic && (*it)->mesh != nullptr && pickingLine.Intersects((*it)->mesh->globalBoundingBox))
				{
					objects.push_back((*it)->myGameObject);
				}
			}

			if (objects.size() > 0)
			{
				App->scene->selectedGO = objects.back();
			}
		}
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
	if (selectedCamera == sceneCamera)
	{
		math::float3 center = math::float3(0.0f, 1.0f, 10.0f);
		if (App->scene->selectedGO != nullptr && App->scene->selectedGO->componentMesh != nullptr)
		{
			center = App->scene->selectedGO->componentMesh->mesh->globalBoundingBox.CenterPoint();
		}

		//Reset all variables (position, front, up, fov, pitch, yaw, firstmouse)
		selectedCamera->frustum.pos = center;
		selectedCamera->frustum.front = math::float3(0.0f, 0.0f, -1.0f);
		selectedCamera->frustum.up = math::float3(0.0f, 1.0f, 0.0f);
		selectedCamera->frustum.verticalFov = math::pi / 4.0f;
		selectedCamera->frustum.horizontalFov = 2.f * atanf(tanf(selectedCamera->frustum.verticalFov * 0.5f) * ((float)screenWidth / (float)screenHeight));
		selectedCamera->fovY = 45.0f;
		selectedCamera->fovX = 45.0f;
		selectedCamera->pitch = 0;
		selectedCamera->yaw = 0;
		selectedCamera->firstMouse = true;

		if (App->scene->selectedGO != nullptr && App->scene->selectedGO->componentMesh != nullptr)
		{
			//Add distance still we can see all the bounding box
			while (!selectedCamera->frustum.Contains(App->scene->selectedGO->componentMesh->mesh->globalBoundingBox))
			{
				selectedCamera->frustum.pos.z += 10;
			}
		}
	}
}


bool ModuleCamera::CleanUp()
{
	selectedCamera = nullptr;
	RELEASE(sceneCamera);
	return true;
}


void ModuleCamera::Move(Directions dir)
{
	switch (dir) {
	case Directions::UP:
		selectedCamera->frustum.pos += selectedCamera->frustum.up.Normalized() * selectedCamera->mSpeed;
		break;
	case Directions::DOWN:
		selectedCamera->frustum.pos -= selectedCamera->frustum.up.Normalized() * selectedCamera->mSpeed;
		break;
	case Directions::FORWARD:
		selectedCamera->frustum.pos += selectedCamera->frustum.front.Normalized() * selectedCamera->mSpeed;
		break;
	case Directions::BACKWARD:
		selectedCamera->frustum.pos -= selectedCamera->frustum.front.Normalized() * selectedCamera->mSpeed;
		break;
	case Directions::LEFT:
		selectedCamera->frustum.pos += selectedCamera->frustum.up.Cross(selectedCamera->frustum.front).Normalized() * selectedCamera->mSpeed;
		break;
	case Directions::RIGHT:
		selectedCamera->frustum.pos -= selectedCamera->frustum.up.Cross(selectedCamera->frustum.front).Normalized() * selectedCamera->mSpeed;
		break;
	}
}


void ModuleCamera::MouseUpdate()
{
	if (selectedCamera == nullptr)
	{
		return;
	}

	iPoint mousePosition = App->input->GetMousePosition();

	if (selectedCamera->firstMouse) {
		selectedCamera->lastX = mousePosition.x;
		selectedCamera->lastY = mousePosition.y;
		selectedCamera->firstMouse = false;
	}

	float xoffset = (float)mousePosition.x - (float)selectedCamera->lastX;
	float yoffset = (float)selectedCamera->lastY - (float)mousePosition.y;
	selectedCamera->lastX = mousePosition.x;
	selectedCamera->lastY = mousePosition.y;

	xoffset *= 0.5f;
	yoffset *= 0.5f;

	selectedCamera->yaw += xoffset;
	selectedCamera->pitch += yoffset;

	selectedCamera->pitch = math::Clamp(selectedCamera->pitch, -89.0f, 89.0f);

	math::float3 front;

	front.x = SDL_sinf(math::DegToRad(selectedCamera->yaw)) * SDL_cosf(math::DegToRad(selectedCamera->pitch));
	front.y = SDL_sinf(math::DegToRad(selectedCamera->pitch));
	front.z = -SDL_cosf(math::DegToRad(selectedCamera->yaw)) * SDL_cosf(math::DegToRad(selectedCamera->pitch));
	selectedCamera->frustum.front = front.Normalized();
}

void ModuleCamera::SetPlaneDistances(float nearDist, float farDist)
{
	if (nearDist > 0.0f && nearDist < selectedCamera->frustum.farPlaneDistance)
	{
		selectedCamera->frustum.nearPlaneDistance = nearDist;
	}

	if (farDist > 0.0f && farDist > selectedCamera->frustum.nearPlaneDistance)
	{
		selectedCamera->frustum.farPlaneDistance = farDist;
	}
}


