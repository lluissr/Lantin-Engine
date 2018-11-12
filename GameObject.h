#ifndef __Gdamed_h__
#define __Gdamed_h__

#include "Component.h"
#include <vector>

class GameObject
{
public:
	void Update();
	Component* CreateComponente();

	char* name;
	std::vector<Component*> components;
	GameObject* parent = nullptr;
	std::vector<GameObject*> gameObjects;
};

#endif
