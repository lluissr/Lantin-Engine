#ifndef __Panel_h__
#define __Panel_h__

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

class Panel
{
public:
	virtual ~Panel()
	{
	};
	virtual void Draw() = 0;
	bool show = false;
};

#endif