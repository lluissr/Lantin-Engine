#ifndef __Component_h__
#define __Component_h__

class GameObject;

enum class ComponentType
{
	EMPTY, MESH, MATERIAL, SOUND, SCRIPT, LIGHT
};

class Component
{
public:
	Component(GameObject* gameObject, ComponentType type) : myGameObject(gameObject), type(type) {}
	virtual ~Component() {}

	bool active = true;
	GameObject* myGameObject = nullptr;
	ComponentType type = ComponentType::EMPTY;

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
};
#endif