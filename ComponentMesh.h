#ifndef __ComponentMesh_h__
#define __ComponentMesh_h__

#include "Component.h"
struct Mesh;

class ComponentMesh : public Component
{
	ComponentMesh(GameObject* parent, ComponentType type);
	~ComponentMesh();

	void Update();

	Mesh* mesh = nullptr;
};

#endif // !__ComponentMesh_h__

