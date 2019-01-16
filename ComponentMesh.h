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

	void SaveJSON(const Config* config) override;
	void LoadJSON(const Config* config, rapidjson::Value& value) override;

	Mesh* mesh = nullptr;
};

#endif // !__ComponentMesh_h__

