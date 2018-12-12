#ifndef __PanelModel_h__
#define __PanelModel_h__

#include "Panel.h"

class PanelModel : public Panel
{
public:
	PanelModel();
	~PanelModel();

	void Draw() override;
};

#endif

