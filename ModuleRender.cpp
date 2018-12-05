#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleProgram.h"
#include "ModuleModelLoader.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
#include "SDL.h"
#include "debugdraw.h"
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
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

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
	glViewport(0, 0, width, height);

	checkersTexture = App->textures->Load("./Textures/checker.jpg");

	InitFrameBuffer(App->camera->screenWidth, App->camera->screenHeight);

	return true;
}

update_status ModuleRender::PreUpdate()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	App->editor->InitImGuiFrame();

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	dd::xzSquareGrid(-100.0f, 100.0f, 0.0f, 1.0f, math::float3(0.65f, 0.65f, 0.65f));
	dd::axisTriad(math::float4x4::identity, 0.125f, 1.25f, 0, false);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	App->debugDraw->Draw(fbo, App->camera->screenWidth, App->camera->screenHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	for (GameObject* gameObject : App->scene->root->gameObjects)
	{
		if (gameObject->isActive)
		{
			RenderGameObject(gameObject);
		}
	}

	if (renderBoundingBox)
	{
		RenderBoundingBox();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return UPDATE_CONTINUE;
}

void  ModuleRender::RenderGameObject(GameObject* gameObject)
{
	if (gameObject->isActive)
	{
		if (gameObject->gameObjects.size() > 0)
		{
			for (GameObject* go : gameObject->gameObjects)
			{
				RenderGameObject(go);
			}
		}

		if (gameObject->mesh != NULL && gameObject->material != NULL) {
			RenderMesh(*gameObject->mesh->mesh, *gameObject->material->material, gameObject->matrix);
		}
	}
}

void ModuleRender::RenderMesh(const Mesh& mesh, const Material& material, math::float4x4 modelMatrix)
{
	unsigned program = 0;
	switch (material.program) {
	case 0:
		program = App->program->program;
		break;
	case 1:
		program = App->program->colorProgram;
		break;
	case 2:
		program = App->program->flatProgram;
		break;
	default:
		program = App->program->program;
		break;
	}

	glUseProgram(program);
	
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, &modelMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, &App->camera->LookAt(App->camera->cameraPosition, App->camera->cameraFront, App->camera->cameraUp)[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, &App->camera->frustum.ProjectionMatrix()[0][0]);

	//TODO: Refactor all with vao
	if (material.program == 0)
	{
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
	else if (material.program == 1)
	{
		glUniform4fv(glGetUniformLocation(program, "newColor"), 1, (float*)&material.color);
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
	else
	{
		glUniform3fv(glGetUniformLocation(program, "light_pos"), 1, (float*)&math::float3(-2.0f, 0.0f, 6.0f));
		glUniform1f(glGetUniformLocation(program, "ambient"), 0.3f);
		glUniform1f(glGetUniformLocation(program, "shininess"), 64.0f);
		glUniform1f(glGetUniformLocation(program, "k_ambient"), 1.0f);
		glUniform1f(glGetUniformLocation(program, "k_diffuse"), 0.5f);
		glUniform1f(glGetUniformLocation(program, "k_specular"), 0.6f);
		glUniform1i(glGetUniformLocation(program, "use_diffuse_map"), 0);
		glUniform4fv(glGetUniformLocation(program, "newColor"), 1, (float*)&material.color);

		glBindVertexArray(mesh.vao);
		glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}
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

	glUseProgram(App->program->colorProgram);

	math::float4x4 model(math::float4x4::identity);

	glUniformMatrix4fv(glGetUniformLocation(App->program->program, "model"), 1, GL_TRUE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->program->program, "view"), 1, GL_TRUE, &App->camera->LookAt(App->camera->cameraPosition, App->camera->cameraFront, App->camera->cameraUp)[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(App->program->program, "proj"), 1, GL_TRUE, &App->camera->frustum.ProjectionMatrix()[0][0]);
	float color[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(App->program->colorProgram, "newColor"), 1, color);

	glDrawArrays(GL_LINES, 0, 24);

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
}

update_status ModuleRender::PostUpdate()
{
	App->editor->EndImGuiFrame();

	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");

	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &rbo);

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

void ModuleRender::DrawCameraWindow()
{
	ImVec2 size = ImGui::GetWindowSize();

	App->camera->WindowResized((unsigned)size.x, (unsigned)size.y);

	ImGui::Image((ImTextureID)App->renderer->renderTexture, { size.x, size.y }, { 0,1 }, { 1,0 });
}

void ModuleRender::InitFrameBuffer(int width, int height)
{
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &rbo);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0
	);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("Framebuffer mal");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
