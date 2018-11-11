#ifndef __ModuleInput_h__
#define __ModuleInput_h__

#include "Module.h"
#include "Globals.h"
#include "Point.h"

typedef unsigned __int8 Uint8;

#define NUM_MOUSE_BUTTONS 5

enum EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};


enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};


class ModuleInput : public Module
{
public:
	
	ModuleInput();
	~ModuleInput();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	bool CleanUp();

	void DrawImGui();

	KeyState GetKey(int id) const
	{
		return keyboard[id];
	}


	KeyState GetMouseButtonDown(int id) const
	{
		return mouse_buttons[id - 1];
	}

	const iPoint& GetMouseMotion() const;
	const iPoint& GetMousePosition() const;

private:
	void HandleDropFile(const char* path);

	KeyState* keyboard;
	KeyState mouse_buttons[NUM_MOUSE_BUTTONS];
	bool windowEvents[WE_COUNT];
	iPoint mouse_motion;
	iPoint mouse;
	
};

#endif