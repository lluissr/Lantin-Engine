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
