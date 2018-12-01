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

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	bool CleanUp() override;

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

