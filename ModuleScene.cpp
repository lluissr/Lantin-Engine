#include "ModuleScene.h"
#include "GameObject.h"
#include "ModuleModelLoader.h"

ModuleScene::ModuleScene()
{
}


ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init()
{
	root = new GameObject();
	root->name = "Scene Root";
	return true;
}

update_status ModuleScene::PreUpdate()
{
	root->RemoveChild();

	if (selectedGO != nullptr && selectedGO->toDelete)
	{
		delete selectedGO;
		selectedGO = nullptr;
	}

	return UPDATE_CONTINUE;
}


update_status ModuleScene::Update()
{
	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	selectedGO = nullptr;
	delete root;
	root = nullptr;

	return true;
}

GameObject* ModuleScene::CreateGameObject()
{
	return nullptr;
}

void ModuleScene::CleanRootGameObjects()
{
	for (std::list<GameObject*>::iterator it = root->gameObjects.begin(); it != root->gameObjects.end(); ++it)
		delete *it;

	root->gameObjects.clear();
}

void ModuleScene::SelectGameObject(GameObject* go)
{
	assert(go != NULL);

	if (selectedGO != nullptr)
		selectedGO->SetIsSelected(false);

	selectedGO = go;
	if (go != nullptr)
		go->SetIsSelected(true);
}

void ModuleScene::CalculateGlobalMatrix(GameObject* go)
{
	if (go->parent == nullptr)
	{
		go->globalMatrix = go->localMatrix;
	}
	else
	{
		go->globalMatrix = go->parent->globalMatrix*go->localMatrix;
	}

	for each (GameObject* gameObject in go->gameObjects)
	{
		CalculateGlobalMatrix(gameObject);
	}

}