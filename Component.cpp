#include "Component.h"
#include "Utils/crossguid/guid.hpp"

Component::Component(GameObject* gameObject, ComponentType type)
{
	myGameObject = gameObject;
	componentType = type;
	uuid = xg::newGuid().str();
}