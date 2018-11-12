#ifndef __Component_h__
#define __Component_h__

#include "GameObject.h"

enum ComponentType
{
	MESH, MATERIAL, SOUND, SCRIPT, LIGHT
};

class Component
{
	bool active;
	GameObject* myGameObject = nullptr;
	ComponentType type;

	virtual void Enable() = 0;
	virtual void Update() = 0;
	virtual void Disable() = 0;
};
#endif