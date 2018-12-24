#include "PanelScene.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleModelLoader.h"


PanelScene::PanelScene()
{
	show = true;
}


PanelScene::~PanelScene()
{
}

void PanelScene::Draw()
{
	ImGui::Begin("Scene information", &show);
	ImGui::SliderFloat("Ambient", &App->scene->ambient, 0.0f, 1.0f);
	ImGui::Text("Light position:");
	ImGui::PushItemWidth(70);
	ImGui::Text("X:");
	ImGui::SameLine();
	ImGui::PushID("1");
	ImGui::InputFloat("", &App->scene->lightPosition.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::SameLine();
	ImGui::PopID();
	ImGui::Text("Y:");
	ImGui::SameLine();
	ImGui::PushID("2");
	ImGui::InputFloat("", &App->scene->lightPosition.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::SameLine();
	ImGui::PopID();
	ImGui::Text("Z:");
	ImGui::SameLine();
	ImGui::PushID("3");
	ImGui::InputFloat("", &App->scene->lightPosition.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::PopID();
	ImGui::PopItemWidth();

	if (ImGui::CollapsingHeader("Draw Primitives"))
	{
		static char str0[128] = "Sphere";
		ImGui::InputText("Sphere name", str0, IM_ARRAYSIZE(str0));
		static ImVec4 color = ImColor(114, 144, 154, 200);
		ImGui::ColorEdit3("Sphere Color", (float*)&color);
		if (ImGui::Button("Draw Sphere"))
		{
			App->modelLoader->LoadSphere(str0, 1.0f, 30, 30, math::float4(color.x, color.y, color.z, color.w));
		}
		ImGui::Separator();
		static char str1[128] = "Torus";
		ImGui::InputText("Torus name", str1, IM_ARRAYSIZE(str0));
		static ImVec4 colorTorus = ImColor(100, 100, 100, 200);
		ImGui::ColorEdit3("Torus Color", (float*)&colorTorus);
		if (ImGui::Button("Draw Torus"))
		{
			App->modelLoader->LoadTorus(str1, 0.5f, 0.67f, 30, 30, math::float4(colorTorus.x, colorTorus.y, colorTorus.z, colorTorus.w));
		}
	}
	if (ImGui::CollapsingHeader("Scene objects"))
	{
		if (ImGui::Button("Add camera"))
		{
			App->scene->CreateCamera();
		}
		if (ImGui::TreeNode(App->scene->root->name.c_str()))
		{
			for (GameObject* gameObject : App->scene->root->gameObjects)
			{
				DrawTreeNode(gameObject);
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();
}


void PanelScene::DrawTreeNode(GameObject* go)
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
	if (App->scene->gameCamera != nullptr && go->uuid == App->scene->gameCamera->uuid)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, { 0,1,0,1 });
	}
	if (!go->isActive)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, { 1,0,0,1 });
	}
	bool opened = ImGui::TreeNodeEx(go->uuid.c_str(), flags, go->name.c_str());
	if (App->scene->gameCamera != nullptr && go->uuid == App->scene->gameCamera->uuid)
	{
		ImGui::PopStyleColor();
	}
	if (ImGui::IsItemClicked(0))
	{
		App->scene->SelectGameObject(go);
	}
	if (ImGui::IsItemClicked(1))
	{
		ImGui::OpenPopup("TreePopup");
	}
	if (ImGui::BeginPopup("TreePopup"))
	{
		if (go->componentCamera != nullptr)
		{
			if (ImGui::Button("Use as game camera"))
			{
				App->scene->UseAsGameCamera(go);
			}
		}
		if (ImGui::Button("Duplicar"))
		{
			GameObject* newGameObject = new GameObject(*go);
			go->parent->gameObjects.push_back(newGameObject);
		}
		if (ImGui::Button("Eliminar"))
		{
			go->MarkToDelete();
		}
		ImGui::EndPopup();
	}
	if (opened)
	{
		for (GameObject* gameObject : go->gameObjects)
		{
			DrawTreeNode(gameObject);
		}
		ImGui::TreePop();
	}
	if (!go->isActive)
	{
		ImGui::PopStyleColor();
	}
}

