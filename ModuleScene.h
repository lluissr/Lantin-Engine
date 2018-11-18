#ifndef __ModuleScene_h__
#define __ModuleScene_h__

#include "Module.h"

class GameObject;

class ModuleScene : public Module 
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init();
	update_status Update();
	bool CleanUp();

	GameObject* CreateGameObject();
	void CleanRootGameObjects();
	void DrawImGui();

	void DrawModelImGui(GameObject* gameObject);

	GameObject* root = nullptr;
	int menuCount = 0;

};

#endif

