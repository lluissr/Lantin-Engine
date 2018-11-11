#pragma once
#include "Module.h"
#include "Globals.h"
#include "ModuleModelLoader.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void RenderMesh(const ModuleModelLoader::Mesh& mesh);
	void RenderBoundingBox();
	void DrawImGui();

	void* context;
	bool renderBoundingBox = false;
	bool useCheckerTexture = false;
	GLuint checkersTexture;
	int currentItemSelected = 0;
};
