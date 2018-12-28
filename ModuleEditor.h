#ifndef __ModuleEditor_h__
#define __ModuleEditor_h__

#include "Module.h"
#include "Globals.h"
#include <vector>
#include "PanelConfiguration.h"
#include "PanelModel.h"
#include "PanelAbout.h"
#include "PanelHardware.h"
#include "PanelConsole.h"
#include "PanelScene.h"
#include "PanelViewport.h"
#include "PanelEditor.h"
#include "PanelLibrary.h"


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
	void InitImGuiFrame();
	void EndImGuiFrame();
	
	PanelConfiguration* panelConfiguration = nullptr;
	PanelModel* panelModel = nullptr;
	PanelAbout* panelAbout = nullptr;
	PanelHardware* panelHardware = nullptr;
	PanelConsole* panelConsole = nullptr;
	PanelScene* panelScene = nullptr;
	PanelViewport* panelViewport = nullptr;
	PanelViewport* panelViewportGame = nullptr;
	PanelEditor* panelEditor = nullptr;
	PanelLibrary* panelLibrary = nullptr;

private:
	const char* glsl_version = "#version 130";

	bool DrawMenu();
	void CreateDockSpace() const;
};

#endif