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

		bool changed = false;
		if (ImGui::TreeNode((void*)(menuCount * 3), "Transformation"))
		{
			ImGui::PushItemWidth(75);
			ImGui::Text("Position:");
			ImGui::Text("X:"); 
			ImGui::SameLine();
			ImGui::PushID("1");
			if (ImGui::InputFloat("", &go->position.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
				changed = true;
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Y:"); 
			ImGui::SameLine();
			ImGui::PushID("2");
			if (ImGui::InputFloat("", &go->position.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
				changed = true;
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Z:"); 
			ImGui::SameLine();
			ImGui::PushID("3");
			if (ImGui::InputFloat("", &go->position.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
				changed = true;
			ImGui::PopID();

			math::float3 rotation = go->rotation.ToEulerXYZ();
			rotation *= 57.295779513082320876f;
			ImGui::Text("Rotation:");
			ImGui::Text("X:"); 
			ImGui::SameLine();
			ImGui::PushID("4");
			if (ImGui::InputFloat("", &rotation.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
				changed = true;
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Y:"); 
			ImGui::SameLine();
			ImGui::PushID("5");
			if (ImGui::InputFloat("", &rotation.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
				changed = true;
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Z:"); 
			ImGui::SameLine();
			ImGui::PushID("6");
			if (ImGui::InputFloat("", &rotation.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
				changed = true;
			ImGui::PopID();
			rotation *= 0.0174532925199432957f;
			go->rotation = go->rotation.FromEulerXYZ(rotation.x, rotation.y, rotation.z);

			ImGui::Text("Scale:");
			ImGui::Text("X:"); 
			ImGui::SameLine();
			ImGui::PushID("7");
			if (ImGui::InputFloat("", &go->scale.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
				changed = true;
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Y:"); 
			ImGui::SameLine();
			ImGui::PushID("8");
			if (ImGui::InputFloat("", &go->scale.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
				changed = true;
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Z:"); 
			ImGui::SameLine();
			ImGui::PushID("9");
			if (ImGui::InputFloat("", &go->scale.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
				changed = true;
			ImGui::PopID();
			ImGui::PopItemWidth();
			ImGui::TreePop();

			if (changed)
			{
				go->matrix.Set(float4x4::FromTRS(go->position, go->rotation, go->scale));
			}

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
