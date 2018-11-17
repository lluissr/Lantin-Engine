#include "GameObject.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
}

Component* GameObject::CreateComponente(ComponentType type)
{
	switch (type)
	{
	case ComponentType::MESH:
		
		break;
	case ComponentType::MATERIAL:
		break;
	}

	return nullptr;
}
