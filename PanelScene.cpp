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
	ImGui::DragFloat("", &App->scene->lightPosition.x, 10.0f, -100000.0f, 100000.0f);
	ImGui::SameLine();
	ImGui::PopID();
	ImGui::Text("Y:");
	ImGui::SameLine();
	ImGui::PushID("2");
	ImGui::DragFloat("", &App->scene->lightPosition.y, 10.0f, -100000.0f, 100000.0f);
	ImGui::SameLine();
	ImGui::PopID();
	ImGui::Text("Z:");
	ImGui::SameLine();
	ImGui::PushID("3");
	ImGui::DragFloat("", &App->scene->lightPosition.z, 10.0f, -100000.0f, 100000.0f);
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
			App->modelLoader->LoadSphere(str0, 100.0f, 30, 30, math::float4(color.x, color.y, color.z, color.w));
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
		ImGui::SameLine();
		if (ImGui::Button("New GameObject"))
		{
			GameObject* go = new GameObject();
			go->name = "New GameObject";
			go->parent = App->scene->root;
			App->scene->root->childrens.push_back(go);
		}
		if (ImGui::TreeNode(App->scene->root->name.c_str()))
		{
			DragAndDrop(App->scene->root);

			for (GameObject* gameObject : App->scene->root->childrens)
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
	if (go->childrens.size() == 0)
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
		App->scene->SelectGameObject(go);
		ImGui::OpenPopup("TreePopup");
	}
	if (go->isSelected)
	{
		if (ImGui::BeginPopup("TreePopup"))
		{
			if (go->componentCamera != nullptr)
			{
				if (ImGui::Button("Use as game camera"))
				{
					App->scene->UseAsGameCamera(go);
				}
			}
			if (ImGui::Button("Add game object children"))
			{
				GameObject* newGo = new GameObject();
				newGo->name = "New GameObject";
				newGo->parent = go;
				go->childrens.push_back(newGo);
			}
			if (ImGui::Button("Duplicar"))
			{
				GameObject* newGameObject = new GameObject(*go);
				go->parent->childrens.push_back(newGameObject);
				if (newGameObject->isStatic && go->componentMesh != nullptr && go->componentMesh->mesh != nullptr)
				{
					App->scene->quadTree.InsertGameObject(newGameObject);
				}
			}
			if (ImGui::Button("Eliminar"))
			{
				go->MarkToDelete();
			}
			ImGui::EndPopup();
		}
	}

	DragAndDrop(go);

	if (opened)
	{
		for (GameObject* gameObject : go->childrens)
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

void PanelScene::DragAndDrop(GameObject* go)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		ImGui::SetDragDropPayload("uuid", go->uuid.c_str(), 37);
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("uuid"))
		{
			std::string uuid((char*)payload->Data);

			GameObject* gameObjectToMove = App->scene->GetGameObjectByUUID(App->scene->root, uuid);

			if (gameObjectToMove != nullptr)
			{
				bool areParentAndChild = gameObjectToMove->CheckIfIsDecendent(*go);

				if (!areParentAndChild)
				{
					gameObjectToMove->parent->childrens.remove(gameObjectToMove);

					gameObjectToMove->parent = go;
					go->childrens.push_back(gameObjectToMove);
				}
				else
				{
					LOG("Is not possible to drop a gameObject into one of is decendants");
				}
			}
		}
	}
}