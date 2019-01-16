#ifndef __ComponentMaterial_h__
#define __ComponentMaterial_h__

#include "Component.h"

enum class MaterialType
{
	DIFFUSE = 0,
	OCCLUSION = 1,
	SPECULAR = 2,
	EMISSIVE = 3
};

class Material;

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(GameObject* parent, ComponentType type);
	~ComponentMaterial();

	void CopyFromComponentMaterial(const ComponentMaterial& componentMaterial);

	void SaveJSON(const Config* config) override;
	void LoadJSON(const Config* config, rapidjson::Value& value) override;

	void Update();

	Material* material = nullptr;
};

#endif //__ComponentMaterial_h__

