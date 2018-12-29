#ifndef __PanelModel_h__
#define __PanelModel_h__

#include "Panel.h"
#include <string>

class PanelModel : public Panel
{
public:
	PanelModel();
	~PanelModel();

	void Draw() override;
	void DrawComboBoxMaterials(const char * id);
	void DrawComboBoxMeshes(const char * id);
};

#endif

