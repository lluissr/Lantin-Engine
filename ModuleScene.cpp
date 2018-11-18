#include "ModuleScene.h"
#include "GameObject.h"


ModuleScene::ModuleScene()
{
}


ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init()
{
	root = new GameObject();
	return true;
}

update_status ModuleScene::Update()
{
	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	delete root;
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
