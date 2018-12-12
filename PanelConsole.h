#ifndef __PanelConsole_h__
#define __PanelConsole_h__
#include "Panel.h"

class PanelConsole : public Panel
{
public:
	PanelConsole();
	~PanelConsole();

	void Draw() override;

	void AddLog(const char* fmt, ...);
	void Clear();

	ImGuiTextBuffer Buf;
	bool ScrollToBottom;
};

#endif

