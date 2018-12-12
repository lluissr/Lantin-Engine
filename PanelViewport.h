#ifndef __PanelViewport_h__
#define __PanelViewport_h__

#include "Panel.h"
class PanelViewport : public Panel
{
public:
	PanelViewport();
	~PanelViewport();

	void Draw() override;
};

#endif

