#include "ModuleScene.h"
#include "GameObject.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
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

void ModuleScene::DrawImGui()
{
	menuCount = 0;
	for (GameObject* gameObject : root->gameObjects)
	{
		ImGui::Text("Model loaded has %d meshes", gameObject->gameObjects.size());
		DrawModelImGui(gameObject);
	}
}

void ModuleScene::DrawModelImGui(GameObject* go)
{
	if (go->mesh != NULL && go->material != NULL) 
	{
		ImGui::NewLine();
		ImGui::Text("Mesh name: %s", go->name.c_str());


		if (ImGui::TreeNode((void*)(menuCount * 3), "Transformation"))
		{
			float pos[3] = { go->position.x, go->position.y,  go->position.z };
			float scaling[3] = { go->scale.x,go->scale.y,  go->scale.z };
			float rotation[3] = { go->rotation.x, go->rotation.y,  go->rotation.z };
			ImGui::Text("Position:");
			ImGui::InputFloat3("", pos, 5, ImGuiInputTextFlags_ReadOnly);
			ImGui::Text("Rotation:");
			ImGui::InputFloat3("", rotation, 5, ImGuiInputTextFlags_ReadOnly);
			ImGui::Text("Scale:");
			ImGui::InputFloat3("", scaling, 5, ImGuiInputTextFlags_ReadOnly);
			ImGui::TreePop();

		}
		if (ImGui::TreeNode((void*)(menuCount * 3 + 1), "Geometry"))
		{
			ImGui::Text("Triangles count: %d", go->mesh->mesh->numVertices / 3);
			ImGui::Text("Vertices count: %d", go->mesh->mesh->numVertices);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode((void*)(menuCount * 3 + 2), "Textures"))
		{
			if (go->material->material->texture0 != 0)
			{
				ImGui::Image((ImTextureID)go->material->material->texture0, ImVec2(200, 200));
				ImGui::Text("Dimensions: %dx%d", go->material->material->width, go->material->material->height);
			}
			ImGui::TreePop();
		}

		++menuCount;
	}

	for (GameObject* gameObject : go->gameObjects)
	{
		DrawModelImGui(gameObject);
	}
}
