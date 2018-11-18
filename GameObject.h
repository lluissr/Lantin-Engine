#ifndef __GameObject_h__
#define __GameObject_h__

#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include <list>

class GameObject
{
public:
	GameObject();
	GameObject(GameObject* parent);
	~GameObject();

	void Update();
	Component* CreateComponent(ComponentType type);

	const char* name = nullptr;
	std::list<Component*> components;
	GameObject* parent = nullptr;
	std::list<GameObject*> gameObjects;
	bool active = true;
	bool isStatic = false;

	ComponentMesh* mesh = nullptr;
	ComponentMaterial* material = nullptr;
};

#endif
