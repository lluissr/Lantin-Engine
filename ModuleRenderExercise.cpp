#include "Application.h"

#include "ModuleRenderExercise.h"
#include "ModuleProgram.h"
#include "ModuleTextures.h"
#include "ModuleCamera.h"

#include "GL/glew.h"
#include "SDL.h"
#include "MathGeoLib.h"

ModuleRenderExercise::ModuleRenderExercise()
{
}

ModuleRenderExercise::~ModuleRenderExercise()
{
}

bool ModuleRenderExercise::Init()
{
	texture = App->textures->Load("../Images/Lenna.png");

	float vertex_buffer_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,

	1.0f, -1.0f, 0.0f,
	1.0f,  1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,

	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	};



	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vbo;
}

update_status ModuleRenderExercise::Update()
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(
		0,                  // attribute 0
		3,                  // number of componentes (3 floats)
		GL_FLOAT,           // data type
		GL_FALSE,           // should be normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)(sizeof(float) * 3 * 6) // buffer offset
	);

	glUseProgram(App->program->program);

	RefenceGround();
	ReferenceAxis();

	float color[4] = { 0.0f, 0.5f, 0.5f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->program, "newColor"), 1, color);

	math::float4x4 model(math::float4x4::identity);

	glUniformMatrix4fv(glGetUniformLocation(App->program->program, "model"), 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->program->program, "view"), 1, GL_TRUE, &App->camera->LookAt(App->camera->target, App->camera->eye, App->camera->up)[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->program->program, "proj"), 1, GL_TRUE, &App->camera->frustum.ProjectionMatrix()[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(App->program->program, "texture0"), 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return UPDATE_CONTINUE;
}

bool ModuleRenderExercise::CleanUp()
{
	if (vbo != 0)
	{
		glDeleteBuffers(1, &vbo);
	}

	return true;
}

void ModuleRenderExercise::RefenceGround()
{
	glLineWidth(1.0f);
	float d = 200.0f;
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->program, "newColor"), 1, color);
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

void ModuleRenderExercise::ReferenceAxis()
{
	glLineWidth(2.0f);

	// red X
	float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->program, "newColor"), 1, red);

	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
	glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);
	glEnd();

	// green Y
	float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->program, "newColor"), 1, green);

	glBegin(GL_LINES);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);
	glEnd();

	// blue Z
	float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->program, "newColor"), 1, blue);

	glBegin(GL_LINES);
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
	glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
	glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);
	glEnd();

	glLineWidth(1.0f);
}

