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

	if (go.componentMesh != NULL)
	{
		componentMesh = (ComponentMesh*)CreateComponent(ComponentType::MESH);
		componentMesh->mesh = go.componentMesh->mesh;
	}
	if (go.componentMaterial != NULL)
	{
		componentMaterial = (ComponentMaterial*)CreateComponent(ComponentType::MATERIAL);
		componentMaterial->material = go.componentMaterial->material;
	}

	for each (GameObject* gameObject in go.gameObjects)
	{
		GameObject* childCopy = new GameObject(*gameObject);
		gameObjects.push_back(childCopy);
	}
}

GameObject::~GameObject()
{
	for (std::list<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		delete *it;

	components.clear();

	for (std::list<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
		delete *it;

	gameObjects.clear();
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
	if (!gameObjects.empty())
	{
		gameObjects.remove_if([](GameObject* i) {return i->toDelete;});

		for each (GameObject* go in gameObjects)
		{
			go->RemoveChild();
		}
	}
}

void GameObject::MarkToDelete()
{
	toDelete = true;
	for each (GameObject* go in gameObjects)
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

	for each (GameObject* go in gameObjects)
	{
		go->UpdateBoundingBox();
	}
}