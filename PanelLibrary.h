#ifndef __PanelLibrary_h__
#define __PanelLibrary_h__
#include "Panel.h"
#include <vector>
#include <string>

class PanelLibrary : public Panel
{
public:
	PanelLibrary();
	~PanelLibrary();

	void Draw() override;
	void DrawTreeNode(const char* name, bool isLeaf);
	void PopupMenu(const char * name);

private:
	bool updateList = false;
};

#endif
