#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ModuleModelLoader.h"


GameObject::GameObject()
{
	uuid = xg::newGuid().str();
}


GameObject::GameObject(const GameObject& go)
{
	uuid = xg::newGuid().str();
	name = go.name;
	parent = go.parent;
	isActive = go.isActive;
	isStatic = go.isStatic;

	position = go.position;
	scale = go.scale;
	rotation = go.rotation;
	localMatrix = go.localMatrix;
	globalMatrix = go.globalMatrix;

	if (go.componentMesh != nullptr)
	{
		componentMesh = (ComponentMesh*)CreateComponent(ComponentType::MESH);
		componentMesh->CopyFromComponentMesh(*go.componentMesh);
	}
	if (go.componentMaterial != nullptr)
	{
		componentMaterial = (ComponentMaterial*)CreateComponent(ComponentType::MATERIAL);
		componentMaterial->CopyFromComponentMaterial(*go.componentMaterial);
	}
	if (go.componentCamera != nullptr)
	{
		componentCamera = (ComponentCamera*)CreateComponent(ComponentType::CAMERA);
		componentCamera->CopyFromComponentCamera(*go.componentCamera);
	}

	for each (GameObject* gameObject in go.childrens)
	{
		GameObject* childCopy = new GameObject(*gameObject);
		childrens.push_back(childCopy);
	}
}

GameObject::~GameObject()
{
	for (std::list<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		delete *it;

	components.clear();

	for (std::list<GameObject*>::iterator it = childrens.begin(); it != childrens.end(); ++it)
		delete *it;

	childrens.clear();
}

void GameObject::Update()
{
}

Component* GameObject::CreateComponent(ComponentType type)
{
	Component* ret = nullptr;

	switch (type)
	{
	case ComponentType::MESH:
		componentMesh = new ComponentMesh(this, type);
		ret = componentMesh;
		break;
	case ComponentType::MATERIAL:
		componentMaterial = new ComponentMaterial(this, type);
		ret = componentMaterial;
		break;
	case ComponentType::CAMERA:
		componentCamera = new ComponentCamera(this, type);
		ret = componentCamera;
		break;
	}

	components.push_back(ret);
	return ret;
}

void GameObject::SetIsSelected(bool selected)
{
	isSelected = selected;

	if (parent != nullptr)
		parent->SetIsSelected(selected);
}

void GameObject::RemoveChild()
{
	if (!childrens.empty())
	{
		childrens.remove_if([](GameObject* i) {return i->toDelete; });

		for each (GameObject* go in childrens)
		{
			go->RemoveChild();
		}
	}
}

void GameObject::MarkToDelete()
{
	toDelete = true;
	for each (GameObject* go in childrens)
	{
		go->MarkToDelete();
	}
}

void GameObject::UpdateBoundingBox()
{
	if (componentMesh != nullptr)
	{
		componentMesh->mesh->globalBoundingBox = componentMesh->mesh->localBoundingBox;
		componentMesh->mesh->globalBoundingBox.TransformAsAABB(globalMatrix);
	}

	for each (GameObject* go in childrens)
	{
		go->UpdateBoundingBox();
	}
}

bool GameObject::AddChild(GameObject* go, std::string& parentUuid)
{
	if (uuid == parentUuid)
	{
		go->parent = this;
		childrens.push_back(go);
		return true;
	}
	else
	{
		for each (GameObject* gameObject in childrens)
		{
			if (gameObject->AddChild(go, parentUuid))
			{
				return true;
			}
		}
	}
	return false;
}


bool GameObject::CheckIfIsDecendent(const GameObject& children)
{
	bool result = false;

	if (children.parent != nullptr)
	{
		if (children.parent == this)
		{
			result = true;
		}
		else
		{
			result = CheckIfIsDecendent(*children.parent);
		}
	}

	return result;
}

void GameObject::SaveJSON(Config* config)
{
	config->StartObject();

	config->AddString("uuid", uuid.c_str());
	config->AddString("name", name.c_str());

	if (parent != nullptr)
	{
		config->AddString("parent", parent->uuid.c_str());
	}

	config->AddBool("isActive", isActive);
	config->AddBool("isStatic", isStatic);

	config->AddFloat3("position", position);
	config->AddFloat3("scale", scale);
	config->AddQuat("rotation", rotation);

	config->StartArray("components");
	for (std::list<Component*>::iterator iterator = components.begin(); iterator != components.end(); iterator++)
	{
		(*iterator)->SaveJSON(config);
	}
	config->EndArray();

	config->EndObject();
}

void GameObject::LoadJSON(Config* config, rapidjson::Value& value)
{
	uuid = std::string(config->GetString("uuid", value));
	name = std::string(config->GetString("name", value));

	isActive = config->GetBool("isActive", value);
	isStatic = config->GetBool("isStatic", value);

	position = config->GetFloat3("position", value);
	scale = config->GetFloat3("scale", value);
	rotation = config->GetQuat("rotation", value);
	localMatrix.Set(float4x4::FromTRS(position, rotation, scale));

	rapidjson::Value components = value["components"].GetArray();
	for (rapidjson::Value::ValueIterator it = components.Begin(); it != components.End(); it++)
	{
		Component* component = CreateComponent(config->GetComponentType("componentType", (*it)));

		if (component != nullptr)
		{
			component->LoadJSON(config, (*it));
		}
	}
}
