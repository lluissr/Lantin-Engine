#ifndef __PanelEditor_h__
#define __PanelEditor_h__
#include "Panel.h"

class PanelEditor :	public Panel
{
public:
	PanelEditor();
	~PanelEditor();

	void Draw() override;
};

#endif

