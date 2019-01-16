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

private:
	void PopupMenu(const char * name);
	void DrawTreeNode(const char* name, bool isLeaf);
	
	bool updateList = false;
};

#endif
