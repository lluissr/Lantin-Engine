#include "ModuleScene.h"
#include "GameObject.h"
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

	return UPDATE_CONTINUE;
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

void ModuleScene::SelectGameObject(GameObject* go)
{
	assert(go != NULL);

	if (selectedGO != nullptr)
		selectedGO->SetIsSelected(false);

	selectedGO = go;
	if (go != nullptr)
		go->SetIsSelected(true);
}

void ModuleScene::DrawGameObjectTreeImGui()
{

	if (ImGui::TreeNode(root->name.c_str()))
	{
		for (GameObject* gameObject : root->gameObjects)
		{	
			DrawModelImGui(gameObject);
		}
		ImGui::TreePop();
	}
}

void ModuleScene::DrawModelImGui(GameObject* go)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
	if (go->isSelected)
	{
		flags = ImGuiTreeNodeFlags_Selected;
	}
	if (go->gameObjects.size() == 0)
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}
	if (!go->isActive)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, { 1,0,0,1 });
	}
	bool opened = ImGui::TreeNodeEx(go->uuid.c_str(), flags, go->name.c_str());
	if (ImGui::IsItemClicked(0))
	{
		SelectGameObject(go);
	}
	if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup("TreePopup");
	}
	if (ImGui::BeginPopup("TreePopup"))
	{
		if (ImGui::Button("Duplicar"))
		{
		
		}
		if (ImGui::Button("Eliminar"))
		{
			go->toDelete = true;
		}
		ImGui::EndPopup();
	}
	if (opened)
	{
		for (GameObject* gameObject : go->gameObjects)
		{
			DrawModelImGui(gameObject);
		}
		ImGui::TreePop();
	}
	if (!go->isActive)
	{
		ImGui::PopStyleColor();
	}
}

void ModuleScene::DrawModelImGui()
{
	if (selectedGO == nullptr)
	{
		return;
	}

	ImGui::Text("Name: %s", selectedGO->name.c_str());
	ImGui::Text("Model selected has %d childs.", selectedGO->gameObjects.size());
	ImGui::NewLine();
	ImGui::Checkbox("Active", &selectedGO->isActive);
	ImGui::Checkbox("Static", &selectedGO->isStatic);
	ImGui::NewLine();
	bool changed = false;
	if (ImGui::CollapsingHeader("Transformation"))
	{
		if (ImGui::Button("Apply identity matrix"))
		{
			selectedGO->matrix = math::float4x4::identity;
			selectedGO->position = { 0.0f,0.0f,0.0f };
			selectedGO->scale = { 1.0f,1.0f,1.0f };
			selectedGO->rotation = { 0.0f,0.0f,0.0f,1.0f };
		}
		ImGui::NewLine();
		ImGui::PushItemWidth(75);
		ImGui::Text("Position:");
		ImGui::Text("X:");
		ImGui::SameLine();
		ImGui::PushID("1");
		if (ImGui::InputFloat("", &selectedGO->position.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:");
		ImGui::SameLine();
		ImGui::PushID("2");
		if (ImGui::InputFloat("", &selectedGO->position.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:");
		ImGui::SameLine();
		ImGui::PushID("3");
		if (ImGui::InputFloat("", &selectedGO->position.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::PopID();

		math::float3 rotation = selectedGO->rotation.ToEulerXYZ();
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
		selectedGO->rotation = selectedGO->rotation.FromEulerXYZ(rotation.x, rotation.y, rotation.z);

		ImGui::Text("Scale:");
		ImGui::Text("X:");
		ImGui::SameLine();
		ImGui::PushID("7");
		if (ImGui::InputFloat("", &selectedGO->scale.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:");
		ImGui::SameLine();
		ImGui::PushID("8");
		if (ImGui::InputFloat("", &selectedGO->scale.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:");
		ImGui::SameLine();
		ImGui::PushID("9");
		if (ImGui::InputFloat("", &selectedGO->scale.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::PopID();
		ImGui::PopItemWidth();

		if (changed)
		{
			selectedGO->matrix.Set(float4x4::FromTRS(selectedGO->position, selectedGO->rotation, selectedGO->scale));
		}

	}

	if (selectedGO->mesh != NULL)
	{
		if (ImGui::CollapsingHeader("Geometry"))
		{
			ImGui::Text("Triangles count: %d", selectedGO->mesh->mesh->numVertices / 3);
			ImGui::Text("Vertices count: %d", selectedGO->mesh->mesh->numVertices);
		}
	}

	if (selectedGO->material != NULL)
	{
		if (ImGui::CollapsingHeader("Textures"))
		{
			if (selectedGO->material->material->texture0 != 0)
			{
				ImGui::Image((ImTextureID)selectedGO->material->material->texture0, ImVec2(200, 200));
				ImGui::Text("Dimensions: %dx%d", selectedGO->material->material->width, selectedGO->material->material->height);
			}
		}
	}

}