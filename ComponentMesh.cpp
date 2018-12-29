#include "ComponentMesh.h"
#include "ModuleModelLoader.h"

ComponentMesh::ComponentMesh(GameObject* parent, ComponentType type) : Component(parent, type)
{
	mesh = nullptr;
}

ComponentMesh::~ComponentMesh()
{
	if (mesh != nullptr)
	{
		delete mesh;
		mesh = nullptr;
	}
}

void ComponentMesh::Update()
{

}

void ComponentMesh::CopyFromComponentMesh(const ComponentMesh& componentMesh)
{
	mesh = new Mesh();
	mesh->vbo = componentMesh.mesh->vbo;
	mesh->ibo = componentMesh.mesh->ibo;
	mesh->material = componentMesh.mesh->material;
	mesh->numVertices = componentMesh.mesh->numVertices;
	mesh->numIndices = componentMesh.mesh->numIndices;
	mesh->normalsOffset = componentMesh.mesh->normalsOffset;
	mesh->vertexSize = componentMesh.mesh->vertexSize;
	mesh->vao = componentMesh.mesh->vao;
	mesh->texturesOffset = componentMesh.mesh->texturesOffset;
	mesh->localBoundingBox = componentMesh.mesh->localBoundingBox;
	mesh->globalBoundingBox = componentMesh.mesh->globalBoundingBox;
}