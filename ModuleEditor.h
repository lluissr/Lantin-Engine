#ifndef __ModuleEditor_h__
#define __ModuleEditor_h__

#include "Module.h"
#include "Globals.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include <vector>

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
	bool showAbout = false;
	bool showHardware = false;
	const char* glsl_version = "#version 130";
	std::vector<float> fps_log;
	int item_current = 1;
	//std::vector<float> ms_log;

};

#endif