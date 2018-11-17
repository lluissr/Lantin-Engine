#ifndef __ComponentMaterial_h__
#define __ComponentMaterial_h__

#include "Component.h"
struct Material;

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(GameObject* parent, ComponentType type);
	~ComponentMaterial();

	void Update();

	Material* material = nullptr;
};

#endif //__ComponentMaterial_h__

