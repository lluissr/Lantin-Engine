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

	void AddLog(const char* fmt, ...);
	void Clear();

	ImGuiIO io;
	bool showConfiguration = true;
	bool showAbout = false;
	bool showHardware = false;
	bool showConsole = true;

	const char* glsl_version = "#version 130";
	std::vector<float> fps_log;
	int currentItemSelected = 0;
	
	ImGuiTextBuffer Buf;
	bool ScrollToBottom;
};

#endif