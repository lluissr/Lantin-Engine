#include "ComponentMaterial.h"
#include "ModuleModelLoader.h"
#include "GameObject.h"

ComponentMaterial::ComponentMaterial(GameObject* parent, ComponentType type) : Component(parent, type)
{
	material = nullptr;
}


ComponentMaterial::~ComponentMaterial()
{
	material = nullptr;
}

void ComponentMaterial::CopyFromComponentMaterial(const ComponentMaterial& componentMaterial)
{
	material = new Material();
	material->diffuseMap = componentMaterial.material->diffuseMap;
	material->diffuseMapName = componentMaterial.material->diffuseMapName;
	material->diffuseColor = componentMaterial.material->diffuseColor;
	material->k_diffuse = componentMaterial.material->k_diffuse;
	material->diffuseWidth = componentMaterial.material->diffuseWidth;
	material->diffuseHeight = componentMaterial.material->diffuseHeight;
	material->occlusionMap = componentMaterial.material->occlusionMap;
	material->occlusionMapName = componentMaterial.material->occlusionMapName;
	material->k_ambient = componentMaterial.material->k_ambient;
	material->ambientWidth = componentMaterial.material->ambientWidth;
	material->ambientHeight = componentMaterial.material->ambientHeight;
	material->specularMap = componentMaterial.material->specularMap;
	material->specularMapName = componentMaterial.material->specularMapName;
	material->specularColor = componentMaterial.material->specularColor;
	material->k_specular = componentMaterial.material->k_specular;
	material->shininess = componentMaterial.material->shininess;
	material->specularWidth = componentMaterial.material->specularWidth;
	material->specularHeight = componentMaterial.material->specularHeight;
	material->emissiveMap = componentMaterial.material->emissiveMap;
	material->emissiveMapName = componentMaterial.material->emissiveMapName;
	material->emissiveColor = componentMaterial.material->emissiveColor;
	material->emissiveWidth = componentMaterial.material->emissiveWidth;
	material->emissiveHeight = componentMaterial.material->emissiveHeight;
}

void ComponentMaterial::SaveJSON(Config * config)
{
	config->StartObject();

	config->AddString("uuid", uuid.c_str());
	config->AddComponentType("componentType", componentType);
	config->AddBool("isActive", active);

	if (myGameObject != nullptr)
	{
		config->AddString("gameObjectParent", myGameObject->uuid.c_str());
	}

	if (material->diffuseMapName != nullptr)
	{
		config->AddString("diffuseMapName", material->diffuseMapName);
	}
	config->AddFloat4("diffuseColor", material->diffuseColor);
	config->AddFloat("k_diffuse", material->k_diffuse);

	if (material->occlusionMapName != nullptr)
	{
		config->AddString("occlusionMapName", material->occlusionMapName);
	}
	config->AddFloat("k_ambient", material->k_ambient);

	if (material->specularMapName != nullptr)
	{
		config->AddString("specularMapName", material->specularMapName);
	}
	config->AddFloat4("specularColor", material->specularColor);
	config->AddFloat("k_specular", material->k_specular);
	config->AddFloat("shininess", material->shininess);

	if (material->emissiveMapName != nullptr)
	{
		config->AddString("emissiveMapName", material->emissiveMapName);
	}
	config->AddFloat4("emissiveColor", material->emissiveColor);

	config->EndObject();
}

void ComponentMaterial::Update()
{

}