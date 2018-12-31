#include "ModuleScene.h"
#include "GameObject.h"
#include "ModuleModelLoader.h"
#include "ModuleCamera.h"


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

	gameCamera = CreateCamera();

	return true;
}


update_status ModuleScene::PreUpdate()
{
	root->RemoveChild();

	if (selectedGO != nullptr && selectedGO->toDelete)
	{
		if (selectedGO->uuid == gameCamera->uuid)
		{
			if (gameCamera->componentCamera->uuid == App->camera->selectedCamera->uuid)
			{
				App->camera->selectedCamera = nullptr;
			}
			gameCamera = nullptr;
		}
		delete selectedGO;
		selectedGO = nullptr;
	}
	else
	{
		if (gameCamera != nullptr && gameCamera->toDelete)
		{
			if (gameCamera->componentCamera->uuid == App->camera->selectedCamera->uuid)
			{
				App->camera->selectedCamera = nullptr;
			}
			delete gameCamera;
			gameCamera = nullptr;
		}
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

void ModuleScene::UseAsGameCamera(GameObject* go)
{
	if (App->camera->selectedCamera == nullptr || (App->scene->gameCamera != nullptr && App->scene->gameCamera->componentCamera->uuid == App->camera->selectedCamera->uuid))
	{
		App->camera->selectedCamera = go->componentCamera;
	}
	App->scene->gameCamera = go;
}


GameObject* ModuleScene::CreateCamera()
{
	GameObject* go = new GameObject();
	go->name = "Game camera";
	go->CreateComponent(ComponentType::CAMERA);
	go->parent = root;
	root->gameObjects.push_back(go);
	return go;
}

void ModuleScene::SaveSceneJSON()
{
	LOG("Starting saving scene")
	Config* config = new Config();
	config->StartObject("scene");
	config->AddFloat("ambientLight", ambient);
	config->AddFloat3("lightPosition", lightPosition);

	if (gameCamera != nullptr)
	{
		config->AddString("gameCamera", gameCamera->uuid.c_str());
	}
	config->StartArray("gameObjects");
	SaveGameObjectsJSON(config, root);
	config->EndArray();
	config->EndObject();

	config->WriteToDisk();
	LOG("Scene saved succesfully: Library/Scene/scene.json");
}

void ModuleScene::SaveGameObjectsJSON(Config* config, GameObject* gameObject)
{
	gameObject->SaveJSON(config);

	if (gameObject->gameObjects.size() > 0)
	{
		for (std::list<GameObject*>::iterator iterator = gameObject->gameObjects.begin(); iterator != gameObject->gameObjects.end(); ++iterator)
		{
			SaveGameObjectsJSON(config, (*iterator));
		}
	}
}