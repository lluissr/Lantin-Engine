#include "ComponentMaterial.h"
#include "ModuleModelLoader.h"
#include "ModuleTextures.h"
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
	material->diffuseMapName = componentMaterial.material->diffuseMapName;
	if (!material->diffuseMapName.empty())
	{
		material->diffuseMap = App->textures->Load(material->diffuseMapName.c_str());
	}
	material->diffuseColor = componentMaterial.material->diffuseColor;
	material->k_diffuse = componentMaterial.material->k_diffuse;
	material->diffuseWidth = componentMaterial.material->diffuseWidth;
	material->diffuseHeight = componentMaterial.material->diffuseHeight;
	material->occlusionMapName = componentMaterial.material->occlusionMapName;
	if (!material->occlusionMapName.empty())
	{
		material->occlusionMap = App->textures->Load(material->occlusionMapName.c_str());
	}
	material->k_ambient = componentMaterial.material->k_ambient;
	material->ambientWidth = componentMaterial.material->ambientWidth;
	material->ambientHeight = componentMaterial.material->ambientHeight;
	material->specularMapName = componentMaterial.material->specularMapName;
	if (!material->specularMapName.empty())
	{
		material->specularMap = App->textures->Load(material->specularMapName.c_str());
	}
	material->specularColor = componentMaterial.material->specularColor;
	material->k_specular = componentMaterial.material->k_specular;
	material->shininess = componentMaterial.material->shininess;
	material->specularWidth = componentMaterial.material->specularWidth;
	material->specularHeight = componentMaterial.material->specularHeight;
	material->emissiveMapName = componentMaterial.material->emissiveMapName;
	if (!material->emissiveMapName.empty())
	{
		material->emissiveMap = App->textures->Load(material->emissiveMapName.c_str());
	}
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

	if (!material->diffuseMapName.empty())
	{
		config->AddString("diffuseMapName", material->diffuseMapName.c_str());
	}
	config->AddFloat4("diffuseColor", material->diffuseColor);
	config->AddFloat("k_diffuse", material->k_diffuse);

	if (!material->occlusionMapName.empty())
	{
		config->AddString("occlusionMapName", material->occlusionMapName.c_str());
	}
	config->AddFloat("k_ambient", material->k_ambient);

	if (!material->specularMapName.empty())
	{
		config->AddString("specularMapName", material->specularMapName.c_str());
	}
	config->AddFloat4("specularColor", material->specularColor);
	config->AddFloat("k_specular", material->k_specular);
	config->AddFloat("shininess", material->shininess);

	if (!material->emissiveMapName.empty())
	{
		config->AddString("emissiveMapName", material->emissiveMapName.c_str());
	}
	config->AddFloat4("emissiveColor", material->emissiveColor);

	config->EndObject();
}

void ComponentMaterial::LoadJSON(Config* config, rapidjson::Value& value)
{
	uuid = std::string(config->GetString("uuid", value));
	active = config->GetBool("isActive", value);

	material = new Material();

	material->diffuseColor = config->GetFloat4("diffuseColor", value);
	material->k_diffuse = config->GetFloat("k_diffuse", value);
	material->k_ambient = config->GetFloat("k_ambient", value);
	material->specularColor = config->GetFloat4("specularColor", value);
	material->k_specular = config->GetFloat("k_specular", value);
	material->shininess = config->GetFloat("shininess", value);
	material->emissiveColor = config->GetFloat4("emissiveColor", value);

	if (value.HasMember("diffuseMapName"))
	{
		material->diffuseMapName = std::string(config->GetString("diffuseMapName", value));
		material->diffuseMap = App->textures->Load(material->diffuseMapName.c_str());
		material->diffuseWidth = App->textures->lastImageInfo.Width;
		material->diffuseHeight = App->textures->lastImageInfo.Height;
	}

	if (value.HasMember("occlusionMapName"))
	{
		material->occlusionMapName = std::string(config->GetString("occlusionMapName", value));
		material->occlusionMap = App->textures->Load(material->occlusionMapName.c_str());
		material->ambientWidth = App->textures->lastImageInfo.Width;
		material->ambientHeight = App->textures->lastImageInfo.Height;
	}

	if (value.HasMember("specularMapName"))
	{
		material->specularMapName = std::string(config->GetString("specularMapName", value));
		material->specularMap = App->textures->Load(material->specularMapName.c_str());
		material->specularWidth = App->textures->lastImageInfo.Width;
		material->specularHeight = App->textures->lastImageInfo.Height;
	}

	if (value.HasMember("emissiveMapName"))
	{
		material->emissiveMapName = std::string(config->GetString("emissiveMapName", value));
		material->emissiveMap = App->textures->Load(material->emissiveMapName.c_str());
		material->emissiveWidth = App->textures->lastImageInfo.Width;
		material->emissiveHeight = App->textures->lastImageInfo.Height;
	}
	

}

void ComponentMaterial::Update()
{

}