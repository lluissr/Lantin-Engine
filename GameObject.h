#ifndef __GameObject_h__
#define __GameObject_h__

#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "MathGeoLib.h"
#include <list>

class GameObject
{
public:
	GameObject();
	GameObject(GameObject* parent);
	~GameObject();

	void Update();
	Component* CreateComponent(ComponentType type);

	std::string name = "";
	std::list<Component*> components;
	GameObject* parent = nullptr;
	std::list<GameObject*> gameObjects;
	bool active = true;
	bool isStatic = false;

	ComponentMesh* mesh = nullptr;
	ComponentMaterial* material = nullptr;

	math::float3 position = { 0.0f,0.0f,0.0f };
	math::float3 scale = { 1.0f,1.0f,1.0f };
	math::Quat rotation = { 1.0f,0.0f,0.0f,0.0f };
	math::float4x4 matrix = math::float4x4::identity;
};

#endif
