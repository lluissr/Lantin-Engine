#ifndef __GameObject_h__
#define __GameObject_h__

#include "Component.h"
#include <vector>

class GameObject
{
public:
	GameObject();
	~GameObject();

	void Update();
	Component* CreateComponente(ComponentType type);

	char* name;
	std::vector<Component*> components;
	GameObject* parent = nullptr;
	std::vector<GameObject*> gameObjects;
};

#endif
