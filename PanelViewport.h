#ifndef __PanelViewport_h__
#define __PanelViewport_h__

#include "Panel.h"
struct FrameBuffer;

class PanelViewport : public Panel
{
public:
	PanelViewport(const char* name);
	~PanelViewport();

	void Draw() override;

	const char* panelName = nullptr;
	FrameBuffer* frameBuffer;
};

#endif

