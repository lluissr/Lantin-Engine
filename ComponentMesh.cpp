#include "Component.h"
#include "ComponentMesh.h"
#include "GameObject.h"

ComponentMesh::ComponentMesh(GameObject* parent, ComponentType type) : Component(parent, type)
{
	mesh = nullptr;
}

ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::Update()
{

}