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

	frameBufferScene.frameBufferType = FrameBufferType::SCENE;
	frameBufferGame.frameBufferType = FrameBufferType::GAME;
	InitFrameBuffer(App->camera->screenWidth, App->camera->screenHeight, frameBufferScene);
	InitFrameBuffer(App->camera->screenWidth, App->camera->screenHeight, frameBufferGame);

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

	DrawInFrameBuffer(frameBufferScene);
	if (App->scene->gameCamera != nullptr && App->scene->gameCamera->isActive)
	{
		DrawInFrameBuffer(frameBufferGame);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferGame.fbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	return UPDATE_CONTINUE;
}

void ModuleRender::DrawInFrameBuffer(FrameBuffer& frameBuffer)
{

	math::float4x4 viewMatrix = math::float4x4::identity;
	math::float4x4 projectionMatrix = math::float4x4::identity;
	switch (frameBuffer.frameBufferType)
	{
	case  FrameBufferType::SCENE:
		viewMatrix = App->camera->sceneCamera->LookAt(App->camera->sceneCamera->frustum.pos, App->camera->sceneCamera->frustum.front, App->camera->sceneCamera->frustum.up);
		projectionMatrix = App->camera->sceneCamera->frustum.ProjectionMatrix();
		break;
	case  FrameBufferType::GAME:
		viewMatrix = App->scene->gameCamera->componentCamera->LookAt(App->scene->gameCamera->componentCamera->frustum.pos, App->scene->gameCamera->componentCamera->frustum.front, App->scene->gameCamera->componentCamera->frustum.up);
		projectionMatrix = App->scene->gameCamera->componentCamera->frustum.ProjectionMatrix();
		break;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (GameObject* gameObject : App->scene->root->gameObjects)
	{
		if (gameObject->isActive)
		{
			RenderGameObject(gameObject, viewMatrix, projectionMatrix, frameBuffer);
		}
	}

	UpdateDrawDebug(frameBuffer, viewMatrix, projectionMatrix);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void ModuleRender::UpdateDrawDebug(FrameBuffer& frameBuffer, math::float4x4 viewMatrix, math::float4x4 projectionMatrix)
{

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

	App->debugDraw->Draw(frameBuffer.fbo, App->camera->screenWidth, App->camera->screenHeight, viewMatrix, projectionMatrix);
}


void  ModuleRender::RenderGameObject(GameObject* gameObject, math::float4x4 viewMatrix, math::float4x4 projectionMatrix, FrameBuffer& frameBuffer)
{
	if (gameObject->isActive)
	{

		if (gameObject->gameObjects.size() > 0)
		{
			for (GameObject* go : gameObject->gameObjects)
			{
				RenderGameObject(go, viewMatrix, projectionMatrix, frameBuffer);
			}
		}

		if (gameObject->componentMesh != nullptr && gameObject->componentMaterial != nullptr) {
			if (App->scene->gameCamera == nullptr || !frustumCulling || !App->scene->gameCamera->isActive ||(App->scene->gameCamera != nullptr && App->scene->gameCamera->componentCamera->frustum.Intersects(gameObject->componentMesh->mesh->globalBoundingBox))) {
				RenderMesh(*gameObject->componentMesh->mesh, *gameObject->componentMaterial->material, gameObject->globalMatrix, viewMatrix, projectionMatrix);
			}
		}

		if (frameBuffer.frameBufferType == FrameBufferType::SCENE && gameObject->componentCamera != nullptr && gameObject->componentCamera->showFrustum)
		{
			ddVec3 color = App->scene->gameCamera != nullptr && App->scene->gameCamera->uuid == gameObject->uuid ? dd::colors::LightGreen : dd::colors::Blue;
			dd::frustum((gameObject->componentCamera->frustum.ProjectionMatrix() * gameObject->componentCamera->LookAt(gameObject->componentCamera->frustum.pos, gameObject->componentCamera->frustum.front, gameObject->componentCamera->frustum.up)).Inverted(), color);
		}
	}
}

void ModuleRender::RenderMesh(const Mesh& mesh, const Material& material, math::float4x4 modelMatrix, math::float4x4 viewMatrix, math::float4x4 projectionMatrix)
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
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, &projectionMatrix[0][0]);

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

	glDeleteFramebuffers(1, &frameBufferScene.fbo);
	glDeleteRenderbuffers(1, &frameBufferScene.rbo);
	glDeleteFramebuffers(1, &frameBufferGame.fbo);
	glDeleteRenderbuffers(1, &frameBufferGame.rbo);

	return true;
}

void ModuleRender::InitFrameBuffer(int width, int height, FrameBuffer& frameBuffer)
{
	glDeleteFramebuffers(1, &frameBuffer.fbo);
	glDeleteRenderbuffers(1, &frameBuffer.rbo);

	glGenFramebuffers(1, &frameBuffer.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.fbo);

	glGenTextures(1, &frameBuffer.renderTexture);
	glBindTexture(GL_TEXTURE_2D, frameBuffer.renderTexture);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer.renderTexture, 0
	);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &frameBuffer.rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, frameBuffer.rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, frameBuffer.rbo);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG("Framebuffer mal");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
