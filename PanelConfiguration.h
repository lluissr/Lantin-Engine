#ifndef __PanelConfiguration_h__
#define __PanelConfiguration_h__

#include "Panel.h"
#include <vector>

#define MEMORY_LOG_SIZE 100

class QuadtreeNode;

class PanelConfiguration : public Panel
{
public:
	PanelConfiguration();
	~PanelConfiguration();

	void Draw() override;

	void addMemory(float memory);
	void DrawQuadtree(QuadtreeNode * node);

	std::vector<float> fps_log;
	std::vector<float> memory_log;
};

#endif

