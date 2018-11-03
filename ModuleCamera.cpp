#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleProgram.h"

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
	frustum.pos = float3::zero;
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 100.0f;
	frustum.verticalFov = math::pi / 4.0f;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * (SCREEN_WIDTH / SCREEN_HEIGHT));

	return true;
}

update_status ModuleCamera::PreUpdate()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
		{
			Move(UP);
		}
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
		{
			Move(DOWN);
		}
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			Move(FORWARD);
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			Move(BACKWARD);
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			Move(LEFT);
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			Move(RIGHT);
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

		//No necessari
		/*if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			Rotate(UP);
		}
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			Rotate(DOWN);
		}
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			Rotate(LEFT);
		}
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			Rotate(RIGHT);
		}*/

		MouseUpdate(App->input->GetMousePosition());
	}

	//if (App->input->GetMouseButtonDown(SDL_BUTTON_X1) == KEY_DOWN) {
	//	Zoom(App->input->GetMousePosition());
	//}
	//else if (App->input->GetMouseButtonDown(SDL_BUTTON_X2) == KEY_DOWN) {
	//	Zoom(App->input->GetMousePosition());
	//}

	return UPDATE_CONTINUE;
}


update_status ModuleCamera::Update()
{
	math::float4x4 model(math::float4x4::identity);
	glUseProgram(App->program->axisProgram);
	glUniformMatrix4fv(glGetUniformLocation(App->program->axisProgram, "model"), 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->program->axisProgram, "view"), 1, GL_TRUE, &App->camera->LookAt(App->camera->cameraPosition, App->camera->cameraFront, App->camera->cameraUp)[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->program->axisProgram, "proj"), 1, GL_TRUE, &App->camera->frustum.ProjectionMatrix()[0][0]);
	RefenceGround();
	ReferenceAxis();
	glDrawArrays(GL_LINES, 0, 1);
	glUseProgram(0);

	return UPDATE_CONTINUE;
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
	case UP:
		cameraPosition += cameraUp.Normalized() * mSpeed;
		break;
	case DOWN:
		cameraPosition -= cameraUp.Normalized() * mSpeed;
		break;
	case FORWARD:
		cameraPosition += cameraFront.Normalized() * mSpeed;
		break;
	case BACKWARD:
		cameraPosition -= cameraFront.Normalized() * mSpeed;
		break;
	case LEFT:
		cameraPosition += cameraUp.Cross(cameraFront).Normalized() * mSpeed;
		break;
	case RIGHT:
		cameraPosition -= cameraUp.Cross(cameraFront).Normalized() * mSpeed;
		break;
	}
}


void ModuleCamera::Rotate(Directions dir) {

	switch (dir) {
	case UP:
		pitch += rSpeed;
		break;
	case DOWN:
		pitch -= rSpeed;
		break;
	case LEFT:
		yaw -= rSpeed;
		break;
	case RIGHT:
		yaw += rSpeed;
		break;
	}

	pitch = math::Clamp(pitch, -89.0f, 89.0f);

	math::float3 front;
	front.x = SDL_cosf(math::DegToRad(yaw)) * SDL_cosf(math::DegToRad(pitch));
	front.y = SDL_sinf(math::DegToRad(pitch));
	front.z = SDL_sinf(math::DegToRad(yaw)) * SDL_cosf(math::DegToRad(pitch));
	cameraFront = front.Normalized();
}


void ModuleCamera::MouseUpdate(const iPoint& mousePosition)
{
	if (firstMouse) {
		lastX = mousePosition.x;
		lastY = mousePosition.y;
		firstMouse = false;
	}

	float xoffset = mousePosition.x - lastX;
	float yoffset = lastY - mousePosition.y;
	lastX = mousePosition.x;
	lastY = mousePosition.y;

	xoffset *= 0.5f;
	yoffset *= 0.5f;

	yaw += xoffset;
	pitch += yoffset;

	pitch = math::Clamp(pitch, -89.0f, 89.0f);

	math::float3 front;
	front.x = SDL_cosf(math::DegToRad(yaw)) * SDL_cosf(math::DegToRad(pitch));
	front.y = SDL_sinf(math::DegToRad(pitch));
	front.z = SDL_sinf(math::DegToRad(yaw)) * SDL_cosf(math::DegToRad(pitch));
	cameraFront = front.Normalized();
}


void ModuleCamera::Zoom(const iPoint& mousePosition)
{
	float yoffset = lastY - mousePosition.y;
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}


void ModuleCamera::SetAspectRatio(float aspect_ratio)
{
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspect_ratio);
}


void ModuleCamera::SetFOV(float fov)
{
	float aspect_ratio = frustum.AspectRatio();

	frustum.verticalFov = math::pi * fov;
	SetAspectRatio(aspect_ratio);
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


void ModuleCamera::RefenceGround()
{
	glLineWidth(1.0f);
	float d = 200.0f;
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->axisProgram, "newColor"), 1, color);
	glBegin(GL_LINES);
	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}
	glEnd();
}

void ModuleCamera::ReferenceAxis()
{
	glLineWidth(2.0f);

	// red X
	float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->axisProgram, "newColor"), 1, red);

	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
	glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);
	glEnd();

	// green Y
	float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->axisProgram, "newColor"), 1, green);

	glBegin(GL_LINES);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);
	glEnd();

	// blue Z
	float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->axisProgram, "newColor"), 1, blue);

	glBegin(GL_LINES);
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
	glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
	glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);
	glEnd();

	glLineWidth(1.0f);
}



