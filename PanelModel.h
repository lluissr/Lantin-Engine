#ifndef __PanelModel_h__
#define __PanelModel_h__

#include "Panel.h"
#include <string>
#include "ComponentMaterial.h"

class PanelModel : public Panel
{
public:
	PanelModel();
	~PanelModel();

	void Draw() override;
	void DrawComboBoxMaterials(MaterialType type);
	void DrawComboBoxMeshes();

	GameObject* go = nullptr;
};

#endif

