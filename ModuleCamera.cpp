#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleWindow.h"

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

update_status ModuleCamera::Update()
{
	float vertex_buffer_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f,  1.0f, 0.0f,
	};
	math::float3 target(1.0f, 1.0f, 1.0f);
	math::float3 eye(0.0f, 0.0f, 9.0f);
	math::float3 up(0.0f, 1.0f, 0.0f);
	math::float4x4 matrix;

	math::float3 f(target - eye); f.Normalize();
	math::float3 s(f.Cross(up)); s.Normalize();
	math::float3 u(s.Cross(f));
	matrix[0][0] = s.x; matrix[0][1] = s.y; matrix[0][2] = s.z;
	matrix[1][0] = u.x; matrix[1][1] = u.y; matrix[1][2] = u.z;
	matrix[2][0] = -f.x; matrix[2][1] = -f.y; matrix[2][2] = -f.z;
	matrix[0][3] = -s.Dot(eye); matrix[1][3] = -u.Dot(eye); matrix[2][3] = f.Dot(eye);
	matrix[3][0] = 0.0f; matrix[3][1] = 0.0f; matrix[3][2] = 0.0f; matrix[3][3] = 1.0f;

	math::float4x4 proj = frustum.ProjectionMatrix();

	math::float4x4 result = proj * matrix;

	math::float4 a(vertex_buffer_data[0], vertex_buffer_data[1], vertex_buffer_data[2], 1);
	math::float4 b(vertex_buffer_data[3], vertex_buffer_data[4], vertex_buffer_data[5], 1);
	math::float4 c(vertex_buffer_data[6], vertex_buffer_data[7], vertex_buffer_data[8], 1);

	a = a * result;
	b = b * result;
	c = c * result;

	vertex_buffer_data[0] = a.x / a.w;
	vertex_buffer_data[1] = a.y / a.w;
	vertex_buffer_data[2] = a.z / a.w;
	vertex_buffer_data[3] = b.x / b.w;
	vertex_buffer_data[4] = b.y / b.w;
	vertex_buffer_data[5] = b.z / b.w;
	vertex_buffer_data[6] = c.x / c.w;
	vertex_buffer_data[7] = c.y / c.w;
	vertex_buffer_data[8] = c.z / c.w;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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

	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	if (vbo != 0)
	{
		glDeleteBuffers(1, &vbo);
	}

	return true;
}

