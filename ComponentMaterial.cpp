#include "ComponentMaterial.h"

ComponentMaterial::ComponentMaterial(GameObject* parent, ComponentType type) : Component(parent, type)
{
	material = nullptr;
}


ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::Update()
{

}