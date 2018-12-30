#pragma once
#include "Module.h"
#include "Globals.h"
#include "ModuleModelLoader.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

enum class FrameBufferType
{
	SCENE,
	GAME
};

struct FrameBuffer
{
	unsigned fbo = 0u;
	unsigned rbo = 0u;
	unsigned renderTexture = 0u;
	FrameBufferType frameBufferType;
};

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	void DrawInFrameBuffer(FrameBuffer& frameBuffer);
	void UpdateDrawDebug(FrameBuffer& frameBuffer, math::float4x4 viewMatrix, math::float4x4 projectionMatrix);
	update_status PostUpdate();
	bool CleanUp();
	void  RenderGameObject(GameObject* gameObject, math::float4x4 viewMatrix, math::float4x4 projectionMatrix, FrameBuffer& frameBuffer);
	void RenderMesh(const Mesh& mesh, const Material& material, math::float4x4 modelMatrix, math::float4x4 viewMatrix, math::float4x4 projectionMatrix);
	void InitFrameBuffer(int width, int height, FrameBuffer& frameBuffer);
	unsigned GenerateFallback();

	void* context;
	FrameBuffer frameBufferScene;
	FrameBuffer frameBufferGame;
	bool renderBoundingBoxes = true;
	int currentItemSelected = 0;
	bool useCheckerTexture = false;
	bool showAxis = true;
	bool showGrid = true;
	bool frustumCulling = true;
	bool sceneFocused = true;

	bool enableVSync = true;

private:
	unsigned fallback = 0;
};
