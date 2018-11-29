#ifndef __ModuleScene_h__
#define __ModuleScene_h__

#include "Module.h"
#include <list>

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
	void DrawGameObjectTreeImGui();
	void DrawModelImGui();
	void SelectGameObject(GameObject* go);
	void DrawModelImGui(GameObject* gameObject);

	GameObject* root = nullptr;
	GameObject* selectedGO = nullptr;
};

#endif

