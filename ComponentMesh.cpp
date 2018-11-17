#include "Component.h"
#include "ComponentMesh.h"

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