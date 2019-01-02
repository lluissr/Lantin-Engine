#ifndef __PanelScene_h__
#define __PanelScene_h__
#include "Panel.h"
#include "GameObject.h"

class PanelScene : public Panel
{
public:
	PanelScene();
	~PanelScene();

	void Draw() override;
	void DrawTreeNode(GameObject* go);
	void DragAndDrop(GameObject* go);
};

#endif

