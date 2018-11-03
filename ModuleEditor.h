#ifndef __ModuleEditor_h__
#define __ModuleEditor_h__

#include "Module.h"
#include "Globals.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	ImGuiIO io;
	bool show = false;
	const char* glsl_version = "#version 130";

};

#endif