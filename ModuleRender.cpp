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
#include "ImGuizmo.h"


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

	SDL_GL_SetSwapInterval(1);

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

	fallback = GenerateFallback();

	frameBufferScene.frameBufferType = FrameBufferType::EDITOR;
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
	if (App->scene->gameCamera != nullptr && App->scene->gameCamera->isActive && App->scene->gameCamera->componentCamera->active)
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
	case  FrameBufferType::EDITOR:
		viewMatrix = App->camera->sceneCamera->LookAt(App->camera->sceneCamera->frustum.pos, App->camera->sceneCamera->frustum.front, App->camera->sceneCamera->frustum.up);
		projectionMatrix = App->camera->sceneCamera->frustum.ProjectionMatrix();
		break;
	case  FrameBufferType::GAME:
		viewMatrix = App->scene->gameCamera->componentCamera->LookAt(App->scene->gameCamera->componentCamera->frustum.pos, App->scene->gameCamera->componentCamera->frustum.front, App->scene->gameCamera->componentCamera->frustum.up);
		projectionMatrix = App->scene->gameCamera->componentCamera->frustum.ProjectionMatrix();
		break;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, msaa ? frameBuffer.msfbo : frameBuffer.fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (frustumCulling && App->scene->gameCamera != nullptr && App->scene->gameCamera->isActive && App->scene->gameCamera->componentCamera->active)
	{
		gameObjectsCollideQuadtree.clear();
		App->scene->quadTree.CollectIntersections(gameObjectsCollideQuadtree, App->scene->gameCamera->componentCamera->frustum);

		for (std::vector<GameObject*>::iterator it = gameObjectsCollideQuadtree.begin(); it != gameObjectsCollideQuadtree.end(); ++it)
		{
			RenderGameObject(*it, viewMatrix, projectionMatrix);
		}

		for (std::vector<ComponentMesh*>::iterator it = meshes.begin(); it != meshes.end(); ++it)
		{
			if (!(*it)->myGameObject->isStatic && (*it)->mesh != nullptr && App->scene->gameCamera->componentCamera->frustum.Intersects((*it)->mesh->globalBoundingBox))
			{
				RenderGameObject((*it)->myGameObject, viewMatrix, projectionMatrix);
			}
		}
	}
	else
	{
		for (std::vector<ComponentMesh*>::iterator it = meshes.begin(); it != meshes.end(); ++it)
		{
			RenderGameObject((*it)->myGameObject, viewMatrix, projectionMatrix);
		}
	}

	if (frameBuffer.frameBufferType == FrameBufferType::EDITOR && App->scene->gameCamera != nullptr && App->scene->gameCamera->componentCamera != nullptr && App->scene->gameCamera->componentCamera->showFrustum)
	{
		dd::frustum((App->scene->gameCamera->componentCamera->frustum.ProjectionMatrix() * App->scene->gameCamera->componentCamera->LookAt(App->scene->gameCamera->componentCamera->frustum.pos, App->scene->gameCamera->componentCamera->frustum.front, App->scene->gameCamera->componentCamera->frustum.up)).Inverted(), dd::colors::LightGreen);
	}

	UpdateDrawDebug(frameBuffer, viewMatrix, projectionMatrix);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (msaa)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer.msfbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer.fbo);
		glBlitFramebuffer(0, 0, App->camera->screenWidth, App->camera->screenHeight, 0, 0, App->camera->screenWidth, App->camera->screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}


void ModuleRender::UpdateDrawDebug(FrameBuffer& frameBuffer, math::float4x4 viewMatrix, math::float4x4 projectionMatrix)
{
	if (frameBuffer.frameBufferType == FrameBufferType::EDITOR && App->scene->drawQuadTree)
	{
		DrawQuadTreeNode(App->scene->quadTree.root);
	}

	if (showGrid)
	{
		dd::xzSquareGrid(-40000.0f, 40000.0f, 0.0f, 100.0f, math::float3(0.65f, 0.65f, 0.65f));
	}
	if (showAxis)
	{
		dd::axisTriad(math::float4x4::identity, 12.5f, 125.0f, 0, false);
	}

	if (renderBoundingBoxes && App->scene->selectedGO != nullptr && App->scene->selectedGO->componentMesh != nullptr && App->scene->selectedGO->componentMesh->mesh != nullptr)
	{
		dd::aabb(App->scene->selectedGO->componentMesh->mesh->globalBoundingBox.minPoint, App->scene->selectedGO->componentMesh->mesh->globalBoundingBox.maxPoint, dd::colors::Yellow);
	}

	App->debugDraw->Draw(frameBuffer.fbo, App->camera->screenWidth, App->camera->screenHeight, viewMatrix, projectionMatrix);
}

void ModuleRender::DrawQuadTreeNode(QuadtreeNode* node)
{
	dd::aabb(node->aabb.minPoint, node->aabb.maxPoint, dd::colors::Red);

	if (node->childs[0] != nullptr)
	{
		for (int i = 0; i < 4; ++i)
		{
			DrawQuadTreeNode(node->childs[i]);
		}
	}
}

void  ModuleRender::RenderGameObject(GameObject* gameObject, math::float4x4 viewMatrix, math::float4x4 projectionMatrix)
{
	if (gameObject->isActive && gameObject->componentMesh->active && gameObject->componentMesh->mesh != nullptr && gameObject->componentMaterial != nullptr) {
		RenderMesh(*gameObject->componentMesh->mesh, *gameObject->componentMaterial->material, gameObject->globalMatrix, viewMatrix, projectionMatrix, gameObject->componentMaterial->active);
	}
}

void ModuleRender::RenderMesh(const Mesh& mesh, const Material& material, math::float4x4 modelMatrix, math::float4x4 viewMatrix, math::float4x4 projectionMatrix, bool active)
{
	if (mesh.useWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (active)
	{
		unsigned program = App->program->blinnProgram;

		glUseProgram(program);

		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, &viewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, &projectionMatrix[0][0]);

		glUniform4fv(glGetUniformLocation(program, "diffuseColor"), 1, (float*)&material.diffuseColor);
		glUniform4fv(glGetUniformLocation(program, "emissiveColor"), 1, (float*)&material.emissiveColor);
		glUniform4fv(glGetUniformLocation(program, "specularColor"), 1, (float*)&material.specularColor);

		glUniform3fv(glGetUniformLocation(program, "light_pos"), 1, &App->scene->lightPosition[0]);
		glUniform1f(glGetUniformLocation(program, "ambient"), App->scene->ambient);
		glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);
		glUniform1f(glGetUniformLocation(program, "k_ambient"), material.k_ambient);
		glUniform1f(glGetUniformLocation(program, "k_diffuse"), material.k_diffuse);
		glUniform1f(glGetUniformLocation(program, "k_specular"), material.k_specular);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.diffuseMap != 0 ? material.diffuseMap : fallback);
		glUniform1i(glGetUniformLocation(program, "diffuseMap"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.emissiveMap != 0 ? material.emissiveMap : fallback);
		glUniform1i(glGetUniformLocation(program, "emissiveMap"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, material.occlusionMap != 0 ? material.occlusionMap : fallback);
		glUniform1i(glGetUniformLocation(program, "occlusionMap"), 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, material.specularMap != 0 ? material.specularMap : fallback);
		glUniform1i(glGetUniformLocation(program, "specularMap"), 3);

	}
	else
	{
		unsigned program = App->program->colorProgram;
		glUseProgram(program);
		glUniform4fv(glGetUniformLocation(program, "newColor"), 1, (float*)&math::float4(0, 0, 0, 0));
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, &viewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, &projectionMatrix[0][0]);

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

	glGenFramebuffers(1, &frameBuffer.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.fbo);
	glGenTextures(1, &frameBuffer.renderTexture);
	glBindTexture(GL_TEXTURE_2D, frameBuffer.renderTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenRenderbuffers(1, &frameBuffer.fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, frameBuffer.fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBuffer.fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer.renderTexture, 0);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//msfbo
	glGenFramebuffers(1, &frameBuffer.msfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.msfbo);

	glGenRenderbuffers(1, &frameBuffer.msfbDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, frameBuffer.msfbDepth);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBuffer.msfbDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenRenderbuffers(1, &frameBuffer.msfboColor);
	glBindRenderbuffer(GL_RENDERBUFFER, frameBuffer.msfboColor);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, frameBuffer.msfboColor);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}

unsigned ModuleRender::GenerateFallback()
{
	char fallbackImage[3] = { GLubyte(255), GLubyte(255), GLubyte(255) };
	unsigned imageName = 0;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &imageName);
	glBindTexture(GL_TEXTURE_2D, imageName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, fallbackImage);

	return imageName;
}

void ModuleRender::DrawImGuizmo(float width, float height)
{
	ImVec2 pos = ImGui::GetWindowPos();
	ImGuizmo::SetRect(pos.x, pos.y, width, height);
	ImGuizmo::SetDrawlist();

	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

	GameObject* selectedGO = App->scene->selectedGO;

	if (selectedGO != nullptr)
	{
		ImGuizmo::Enable(true);

		math::float4x4 model = selectedGO->globalMatrix;
		math::float4x4 viewScene = App->camera->sceneCamera->LookAt(App->camera->sceneCamera->frustum.pos, App->camera->sceneCamera->frustum.front, App->camera->sceneCamera->frustum.up);
		math::float4x4 projectionScene = App->camera->sceneCamera->frustum.ProjectionMatrix();

		ImGuizmo::SetOrthographic(false);

		model.Transpose();
		viewScene.Transpose();
		projectionScene.Transpose();
		ImGuizmo::Manipulate((float*)&viewScene, (float*)&projectionScene, mCurrentGizmoOperation, mCurrentGizmoMode, (float*)&model, NULL, NULL, NULL, NULL);

		if (ImGuizmo::IsUsing())
		{
			model.Transpose();
			model.Decompose(selectedGO->position, selectedGO->rotation, selectedGO->scale);
			selectedGO->eulerRotation = selectedGO->rotation.ToEulerXYZ();
			selectedGO->eulerRotation.x = math::RadToDeg(selectedGO->eulerRotation.x);
			selectedGO->eulerRotation.y = math::RadToDeg(selectedGO->eulerRotation.y);
			selectedGO->eulerRotation.z = math::RadToDeg(selectedGO->eulerRotation.z);
			selectedGO->localMatrix.Set(float4x4::FromTRS(selectedGO->position, selectedGO->rotation, selectedGO->scale));
			App->scene->CalculateGlobalMatrix(selectedGO);
			selectedGO->UpdateBoundingBox();
			if (selectedGO->isStatic)
			{
				App->scene->quadTree.RemoveGameObject(selectedGO);
				App->scene->quadTree.InsertGameObject(selectedGO);
			}
		}
	}
}