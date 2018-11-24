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
	void DrawGameObjectTreeImGui();
	void DrawModelImGui();
	void SelectGameObject(GameObject* go);
	void DrawModelImGui(GameObject* gameObject);

	GameObject* root = nullptr;
	GameObject* selectedGO = nullptr;
	bool useIdentityMatrix = false;
};

#endif

