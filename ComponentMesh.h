#ifndef __ComponentMesh_h__
#define __ComponentMesh_h__

#include "Component.h"

class Mesh;

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* parent, ComponentType type);
	~ComponentMesh();

	void Update();
	void CopyFromComponentMesh(const ComponentMesh& componentMesh);
	void SaveJSON(Config* config) override;

	Mesh* mesh = nullptr;
};

#endif // !__ComponentMesh_h__

