#ifndef __ModuleEditor_h__
#define __ModuleEditor_h__

#include "Module.h"
#include "Globals.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include <vector>


#define MEMORY_LOG_SIZE 100

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
	void addMemory(float memory);

	ImGuiIO io;

private:
	const char* glsl_version = "#version 130";
	std::vector<float> fps_log;
	std::vector<float> memory_log;

	ImGuiTextBuffer Buf;
	bool ScrollToBottom;

	bool showConfiguration = false;
	bool showModel = true;
	bool showAbout = false;
	bool showHardware = false;
	bool showConsole = true;

	bool DrawMenu();
	void DrawConsole();
	void DrawHardware();
	void DrawAbout();
	void DrawModel();
	void DrawConfiguration();
};

#endif