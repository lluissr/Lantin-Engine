#ifndef __GameObject_h__
#define __GameObject_h__

#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "MathGeoLib.h"
#include <list>
#include "Utils/crossguid/guid.hpp"

class GameObject
{
public:
	GameObject();
	GameObject(const GameObject& go);
	~GameObject();

	void Update();
	Component* CreateComponent(ComponentType type);
	void SetIsSelected(bool selected);
	void RemoveChild();

	std::string name = "";
	std::string uuid = "";
	std::list<Component*> components;
	GameObject* parent = nullptr;
	std::list<GameObject*> gameObjects;
	bool isActive = true;
	bool isStatic = false;
	bool isSelected = false;
	bool toDelete = false;

	ComponentMesh* mesh = nullptr;
	ComponentMaterial* material = nullptr;

	math::float3 position = { 0.0f,0.0f,0.0f };
	math::float3 scale = { 1.0f,1.0f,1.0f };
	math::Quat rotation = { 0.0f,0.0f,0.0f,1.0f };
	math::float4x4 matrix = math::float4x4::identity;

	bool operator == (const GameObject& go) const { return go.uuid == go.uuid; }
	bool operator != (const GameObject& go) const { return !operator==(go); }
};

#endif
