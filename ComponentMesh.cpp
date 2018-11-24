#include "ComponentMesh.h"

ComponentMesh::ComponentMesh(GameObject* parent, ComponentType type) : Component(parent, type)
{
	mesh = nullptr;
}

ComponentMesh::~ComponentMesh()
{
	mesh = nullptr;
}

void ComponentMesh::Update()
{

}