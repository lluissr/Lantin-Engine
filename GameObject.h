#ifndef __GameObject_h__
#define __GameObject_h__

#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "MathGeoLib.h"
#include <list>
#include "Utils/crossguid/guid.hpp"
#include "Config.h"

class GameObject
{
public:
	GameObject();
	GameObject(const GameObject& go);
	~GameObject();

	void Update();
	Component* CreateComponent(ComponentType type);
	void SetIsSelected(bool selected);
	bool AddChild(GameObject* go, std::string& parentUuid);
	void RemoveChild();
	void RemoveComponent();
	void MarkToDelete();
	void UpdateBoundingBox();
	bool CheckIfIsDecendent(const GameObject& children);

	void SaveJSON(const Config* config);
	void LoadJSON(const Config* config, rapidjson::Value& value);


	std::string name = "";
	std::string uuid = "";
	std::list<Component*> components;
	GameObject* parent = nullptr;
	std::list<GameObject*> childrens;
	bool isActive = true;
	bool isStatic = false;
	bool isSelected = false;
	bool toDelete = false;

	ComponentMesh* componentMesh = nullptr;
	ComponentMaterial* componentMaterial = nullptr;
	ComponentCamera* componentCamera = nullptr;

	math::float3 position = { 0.0f,0.0f,0.0f };
	math::float3 scale = { 1.0f,1.0f,1.0f };
	math::Quat rotation = { 0.0f,0.0f,0.0f,1.0f };
	math::float3 eulerRotation = { 0.0f, 0.0f, 0.0f };
	math::float4x4 localMatrix = math::float4x4::identity;
	math::float4x4 globalMatrix = math::float4x4::identity;

	bool operator == (const GameObject& go) const { return go.uuid == go.uuid; }
	bool operator != (const GameObject& go) const { return !operator==(go); }
};

#endif
