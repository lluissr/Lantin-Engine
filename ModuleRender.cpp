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

update_status ModuleRender::Update()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	UpdateDrawDebug();

	for (GameObject* gameObject : App->scene->root->gameObjects)
	{
		if (gameObject->isActive)
		{
			RenderGameObject(gameObject);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return UPDATE_CONTINUE;
}


void ModuleRender::UpdateDrawDebug()
{
	App->debugDraw->Draw(fbo, App->camera->screenWidth, App->camera->screenHeight);
	if (showGrid)
	{
		dd::xzSquareGrid(-100.0f, 100.0f, 0.0f, 1.0f, math::float3(0.65f, 0.65f, 0.65f));
	}
	if (showAxis)
	{
		dd::axisTriad(math::float4x4::identity, 0.125f, 1.25f, 0, false);
	}

	if (App->scene->selectedGO != nullptr && App->scene->selectedGO->componentMesh != nullptr)
	{
		dd::aabb(App->scene->selectedGO->componentMesh->mesh->globalBoundingBox.minPoint, App->scene->selectedGO->componentMesh->mesh->globalBoundingBox.maxPoint, dd::colors::Yellow);
	}
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

		if (gameObject->componentMesh != NULL && gameObject->componentMaterial != NULL) {
			RenderMesh(*gameObject->componentMesh->mesh, *gameObject->componentMaterial->material, gameObject->globalMatrix);
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
	case 3:
		program = App->program->gouraudProgram;
		break;
	case 4:
		program = App->program->phongProgram;
		break;
	case 5:
		program = App->program->blinnProgram;
		break;
	default:
		program = App->program->program;
		break;
	}

	glUseProgram(program);
	
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, &modelMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, &App->camera->LookAt(App->camera->cameraPosition, App->camera->cameraFront, App->camera->cameraUp)[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, &App->camera->frustum.ProjectionMatrix()[0][0]);

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
		glUniform3fv(glGetUniformLocation(program, "light_pos"), 1, (float*)&App->scene->lightPosition);
		glUniform1f(glGetUniformLocation(program, "ambient"), App->scene->ambient);
		glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);
		glUniform1f(glGetUniformLocation(program, "k_ambient"), material.k_ambient);
		glUniform1f(glGetUniformLocation(program, "k_diffuse"), material.k_diffuse);
		glUniform1f(glGetUniformLocation(program, "k_specular"), material.k_specular);
		glUniform4fv(glGetUniformLocation(program, "newColor"), 1, (float*)&material.color);
	}

	glBindVertexArray(mesh.vao);
	glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
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
