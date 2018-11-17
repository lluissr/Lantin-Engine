#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleProgram.h"
#include "ModuleModelLoader.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleTextures.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "SDL.h"
#include "GL/glew.h"


ModuleRender::ModuleRender()
{
}

// Destructor
ModuleRender::~ModuleRender()
{
}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	context = SDL_GL_CreateContext(App->window->window);

	glewInit();

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_TEXTURE_2D);

	glClearDepth(1.0f);
	glClearColor(0.3f, 0.3f, 0.3f, 1.f);

	int width, height;
	SDL_GetWindowSize(App->window->window, &width, &height);

	checkersTexture = App->textures->Load("./Textures/checker.jpg");

	return true;
}

update_status ModuleRender::PreUpdate()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	//for (unsigned i = 0; i < App->modelLoader->meshes.size(); ++i)
	//{
	//	RenderMesh(*App->modelLoader->meshes[i]);
	//}

	for (GameObject* gameObject : App->modelLoader->parentGameObject->gameObjects)
	{
		RenderMesh(*gameObject->mesh->mesh);
	}

	if (renderBoundingBox)
	{
		RenderBoundingBox();
	}

	return UPDATE_CONTINUE;
}


void ModuleRender::RenderMesh(const Mesh& mesh)
{
	Material& material = App->modelLoader->materials[mesh.material];
	unsigned program = App->program->program;

	glUseProgram(program);

	math::float4x4 model(math::float4x4::identity);
	glUniformMatrix4fv(glGetUniformLocation(App->program->program, "model"), 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->program->program, "view"), 1, GL_TRUE, &App->camera->LookAt(App->camera->cameraPosition, App->camera->cameraFront, App->camera->cameraUp)[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->program->program, "proj"), 1, GL_TRUE, &App->camera->frustum.ProjectionMatrix()[0][0]);

	glActiveTexture(GL_TEXTURE0);
	if (useCheckerTexture)
	{
		glBindTexture(GL_TEXTURE_2D, checkersTexture);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, material.texture0);
	}
	glUniform1i(glGetUniformLocation(program, "texture0"), 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 3 * mesh.numVertices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, nullptr);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void ModuleRender::RenderBoundingBox() const
{
	math::AABB boundingBox = math::AABB(App->modelLoader->minPoint, App->modelLoader->maxPoint);
	math::float3 corner1 = boundingBox.CornerPoint(0);
	math::float3 corner2 = boundingBox.CornerPoint(1);
	math::float3 corner3 = boundingBox.CornerPoint(2);
	math::float3 corner4 = boundingBox.CornerPoint(3);
	math::float3 corner5 = boundingBox.CornerPoint(4);
	math::float3 corner6 = boundingBox.CornerPoint(5);
	math::float3 corner7 = boundingBox.CornerPoint(6);
	math::float3 corner8 = boundingBox.CornerPoint(7);

	float vertex_buffer_data[] = {
	corner1.x, corner1.y, corner1.z,
	corner2.x, corner2.y, corner2.z,

	corner1.x, corner1.y, corner1.z,
	corner3.x, corner3.y, corner3.z,

	corner2.x, corner2.y, corner2.z,
	corner4.x, corner4.y, corner4.z,

	corner3.x, corner3.y, corner3.z,
	corner4.x, corner4.y, corner4.z,

	corner1.x, corner1.y, corner1.z,
	corner5.x, corner5.y, corner5.z,

	corner3.x, corner3.y, corner3.z,
	corner7.x, corner7.y, corner7.z,

	corner2.x, corner2.y, corner2.z,
	corner6.x, corner6.y, corner6.z,

	corner4.x, corner4.y, corner4.z,
	corner8.x, corner8.y, corner8.z,

	corner7.x, corner7.y, corner7.z,
	corner8.x, corner8.y, corner8.z,

	corner6.x, corner6.y, corner6.z,
	corner8.x, corner8.y, corner8.z,

	corner7.x, corner7.y, corner7.z,
	corner5.x, corner5.y, corner5.z,

	corner6.x, corner6.y, corner6.z,
	corner5.x, corner5.y, corner5.z,
	};

	/*float vertex_buffer_data[] = {
	corner1.x, corner1.y, corner1.z,
	corner2.x, corner2.y, corner2.z,
	corner3.x, corner3.y, corner3.z,
	corner4.x, corner4.y, corner4.z,
	corner5.x, corner5.y, corner5.z,
	corner6.x, corner6.y, corner6.z,
	corner7.x, corner7.y, corner7.z,
	corner8.x, corner8.y, corner8.z,
	};

	float index_buffer_data[] = {
		0,1,
		0,2,
		1,3,
		2,3,
		0,4,
		1,5,
		2,6,
		3,7,
		4,6,
		4,5,
		6,7,
		5,7,
	};*/

	unsigned vbo = 0;
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

	glUseProgram(App->program->axisProgram);

	math::float4x4 model(math::float4x4::identity);

	glUniformMatrix4fv(glGetUniformLocation(App->program->program, "model"), 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->program->program, "view"), 1, GL_TRUE, &App->camera->LookAt(App->camera->cameraPosition, App->camera->cameraFront, App->camera->cameraUp)[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->program->program, "proj"), 1, GL_TRUE, &App->camera->frustum.ProjectionMatrix()[0][0]);
	float color[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->axisProgram, "newColor"), 1, color);

	glDrawArrays(GL_LINES, 0, 24);

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
}

update_status ModuleRender::PostUpdate()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (App->editor->io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	SDL_GL_SwapWindow(App->window->window);


	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");

	//Destroy window

	return true;
}

void ModuleRender::DrawImGui()
{
	ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_FirstUseEver);
	if (ImGui::CollapsingHeader("Render"))
	{
		ImGui::Checkbox("Bounding Box", &App->renderer->renderBoundingBox);

		ImGui::Checkbox("Checkers texture ", &App->renderer->useCheckerTexture);

		const char* items[] = { "Backer House", "T-Rex", "Radioactive Barrel" };
		if (ImGui::Combo("Models", &currentItemSelected, items, IM_ARRAYSIZE(items)))
		{
			if (App->modelLoader->modelRendered != currentItemSelected)
			{
				App->modelLoader->CleanModel();
				App->modelLoader->ChooseModelToRender(currentItemSelected);
			}
		}
	}
}
