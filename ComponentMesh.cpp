#include "ComponentMesh.h"
#include "ModuleModelLoader.h"
#include "GameObject.h"
#include "ModuleRender.h"

ComponentMesh::ComponentMesh(GameObject* parent, ComponentType type) : Component(parent, type)
{
	mesh = nullptr;
}

ComponentMesh::~ComponentMesh()
{
	if (mesh != nullptr)
	{
		App->modelLoader->Unload(mesh->meshName.c_str());
		mesh = nullptr;
	}
	std::vector<ComponentMesh*>::iterator it = std::find(App->renderer->meshes.begin(), App->renderer->meshes.end(), this);
	if (it != App->renderer->meshes.end())
	{
		App->renderer->meshes.erase(it);
	}
}

void ComponentMesh::Update()
{

}

void ComponentMesh::CopyFromComponentMesh(const ComponentMesh& componentMesh)
{
	active = componentMesh.active;
	if (componentMesh.mesh != nullptr)
	{
		mesh = App->modelLoader->Load(componentMesh.mesh->meshName.c_str());
		mesh->localBoundingBox = componentMesh.mesh->localBoundingBox;
		mesh->globalBoundingBox = componentMesh.mesh->globalBoundingBox;
	}
}

void ComponentMesh::SaveJSON(Config * config)
{
	config->StartObject();

	config->AddString("uuid", uuid.c_str());
	config->AddComponentType("componentType", componentType);
	config->AddBool("isActive", active);

	if (myGameObject != nullptr)
	{
		config->AddString("gameObjectParent", myGameObject->uuid.c_str());
	}

	if (mesh != nullptr)
	{
		if (!mesh->meshName.empty())
		{
			config->AddString("meshName", mesh->meshName.c_str());
		}
		config->AddBool("useWireframe", mesh->useWireframe);
	}

	config->EndObject();
}

void ComponentMesh::LoadJSON(Config* config, rapidjson::Value& value)
{
	uuid = std::string(config->GetString("uuid", value));
	active = config->GetBool("isActive", value);

	if (value.HasMember("useWireframe"))
	{
		if (value.HasMember("meshName"))
		{
			mesh = App->modelLoader->Load(config->GetString("meshName",value));
			mesh->useWireframe = config->GetBool("useWireframe", value);
		}
		else 
		{
			mesh = new Mesh();
			mesh->useWireframe = config->GetBool("useWireframe", value);
		}
	}
}
