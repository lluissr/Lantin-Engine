#ifndef __ModuleScene_h__
#define __ModuleScene_h__

#include "Module.h"
#include "MathGeoLib.h"
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

	math::float3 lightPosition = math::float3(-2.0f, 0.0f, 6.0f);
	float ambient = 0.3f;
};

#endif

