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
		speed = 1.0f;
	}
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP)
	{
		speed = 0.5f;
	}

	return UPDATE_CONTINUE;
}


update_status ModuleCamera::Update()
{
	math::float4x4 model(math::float4x4::identity);
	glUseProgram(App->program->axisProgram);
	glUniformMatrix4fv(glGetUniformLocation(App->program->axisProgram, "model"), 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->program->axisProgram, "view"), 1, GL_TRUE, &App->camera->LookAt(App->camera->target, App->camera->eye, App->camera->up)[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->program->axisProgram, "proj"), 1, GL_TRUE, &App->camera->frustum.ProjectionMatrix()[0][0]);
	RefenceGround();
	ReferenceAxis();
	glDrawArrays(GL_LINES, 0, 1);

	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	return true;
}


math::float4x4 ModuleCamera::LookAt(math::float3& target, math::float3& eye, math::float3& up)
{
	math::float4x4 matrix;

	forward = math::float3(target - eye);
	forward.Normalize();
	side = math::float3(forward.Cross(up));
	side.Normalize();
	upLU = math::float3(side.Cross(forward));
	matrix[0][0] = side.x; matrix[0][1] = side.y; matrix[0][2] = side.z;
	matrix[1][0] = upLU.x; matrix[1][1] = upLU.y; matrix[1][2] = upLU.z;
	matrix[2][0] = -forward.x; matrix[2][1] = -forward.y; matrix[2][2] = -forward.z;
	matrix[0][3] = -side.Dot(eye); matrix[1][3] = -upLU.Dot(eye); matrix[2][3] = forward.Dot(eye);
	matrix[3][0] = 0.0f; matrix[3][1] = 0.0f; matrix[3][2] = 0.0f; matrix[3][3] = 1.0f;
	return matrix;
}


void ModuleCamera::Move(Directions dir)
{
	switch (dir) {
	case UP:
		eye += math::float3(0.0f, 1.0f, 0.0f) * speed;
		target += math::float3(0.0f, 1.0f, 0.0f) * speed;
		break;
	case DOWN:
		eye -= math::float3(0.0f, 1.0f, 0.0f) * speed;
		target -= math::float3(0.0f, 1.0f, 0.0f) * speed;
		break;
	case FORWARD:
		eye += forward * speed;
		target += forward * speed;
		break;
	case BACKWARD:
		eye -= forward * speed;
		target -= forward * speed;
		break;
	case LEFT:
		eye -= side * speed;
		target -= side * speed;
		break;
	case RIGHT:
		eye += side * speed;
		target += side * speed;
		break;
	}
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



