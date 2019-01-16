#ifndef __Component_h__
#define __Component_h__
#include <string>
#include "Config.h"

class GameObject;

enum class ComponentType
{
	EMPTY, MESH, MATERIAL, CAMERA, SOUND, SCRIPT, LIGHT
};

class Component
{
public:
	Component(GameObject* gameObject, ComponentType type);
	virtual ~Component() {}

	std::string uuid = "";
	bool active = true;
	GameObject* myGameObject = nullptr;
	ComponentType componentType = ComponentType::EMPTY;
	bool toDelete = false;

	virtual void Enable()
	{
		active = true;
	};
	virtual void Update()
	{
	};
	virtual void Disable()
	{
		active = false;
	};

	virtual void SaveJSON(const Config* config) {};
	virtual void LoadJSON(const Config* config, rapidjson::Value& value) {};
};
#endif