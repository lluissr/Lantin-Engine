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
	RELEASE(mesh)
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
	mesh->meshName = componentMesh.mesh->meshName;

	unsigned bytes = sizeof(float)*mesh->numVertices * 3;
	mesh->vertices = new float[mesh->numVertices * 3];
	memcpy(mesh->vertices, componentMesh.mesh->vertices, bytes);

	bytes = sizeof(unsigned)*mesh->numIndices;
	mesh->indices = new unsigned[mesh->numIndices];
	memcpy(mesh->indices, componentMesh.mesh->indices, bytes);
	
	bytes = sizeof(float)*mesh->normalsOffset * 3;
	mesh->normals = new float[mesh->normalsOffset * 3];
	memcpy(mesh->normals, componentMesh.mesh->normals, bytes);

	bytes = sizeof(float)*mesh->texturesOffset * 2;
	mesh->texCoords = new float[mesh->texturesOffset * 2];
	memcpy(mesh->texCoords, componentMesh.mesh->texCoords, bytes);
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
