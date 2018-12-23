#include "PanelModel.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ModuleModelLoader.h"

PanelModel::PanelModel()
{
	show = true;
}


PanelModel::~PanelModel()
{
}

void PanelModel::Draw()
{
	
	ImGui::Begin("Model selected", &show);

	if (App->scene->selectedGO == nullptr)
	{
		ImGui::End();
		return;
	}

	ImGui::Text("Name: %s", App->scene->selectedGO->name.c_str());
	ImGui::Text("Model selected has %d childs.", App->scene->selectedGO->gameObjects.size());
	ImGui::NewLine();
	ImGui::Checkbox("Active", &App->scene->selectedGO->isActive);
	ImGui::Checkbox("Static", &App->scene->selectedGO->isStatic);
	ImGui::NewLine();
	bool changed = false;
	if (ImGui::CollapsingHeader("Transformation"))
	{
		if (ImGui::Button("Apply identity matrix"))
		{
			App->scene->selectedGO->localMatrix = math::float4x4::identity;
			App->scene->selectedGO->position = { 0.0f,0.0f,0.0f };
			App->scene->selectedGO->scale = { 1.0f,1.0f,1.0f };
			App->scene->selectedGO->rotation = { 0.0f,0.0f,0.0f,1.0f };
			App->scene->CalculateGlobalMatrix(App->scene->selectedGO);
			App->scene->selectedGO->UpdateBoundingBox();
		}
		ImGui::NewLine();
		ImGui::PushItemWidth(75);
		ImGui::Text("Position:");
		ImGui::Text("X:");
		ImGui::SameLine();
		ImGui::PushID("1");
		if (ImGui::InputFloat("", &App->scene->selectedGO->position.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:");
		ImGui::SameLine();
		ImGui::PushID("2");
		if (ImGui::InputFloat("", &App->scene->selectedGO->position.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:");
		ImGui::SameLine();
		ImGui::PushID("3");
		if (ImGui::InputFloat("", &App->scene->selectedGO->position.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::PopID();

		math::float3 rotation = App->scene->selectedGO->rotation.ToEulerXYZ();
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
		App->scene->selectedGO->rotation = App->scene->selectedGO->rotation.FromEulerXYZ(rotation.x, rotation.y, rotation.z);

		ImGui::Text("Scale:");
		ImGui::Text("X:");
		ImGui::SameLine();
		ImGui::PushID("7");
		if (ImGui::InputFloat("", &App->scene->selectedGO->scale.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Y:");
		ImGui::SameLine();
		ImGui::PushID("8");
		if (ImGui::InputFloat("", &App->scene->selectedGO->scale.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::SameLine();
		ImGui::PopID();
		ImGui::Text("Z:");
		ImGui::SameLine();
		ImGui::PushID("9");
		if (ImGui::InputFloat("", &App->scene->selectedGO->scale.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			changed = true;
		ImGui::PopID();
		ImGui::PopItemWidth();

		if (changed)
		{
			App->scene->selectedGO->localMatrix.Set(float4x4::FromTRS(App->scene->selectedGO->position, App->scene->selectedGO->rotation, App->scene->selectedGO->scale));
			App->scene->CalculateGlobalMatrix(App->scene->selectedGO);
			App->scene->selectedGO->UpdateBoundingBox();
		}

	}

	if (App->scene->selectedGO->componentMesh != NULL)
	{
		if (ImGui::CollapsingHeader("Geometry"))
		{
			ImGui::Text("Triangles count: %d", App->scene->selectedGO->componentMesh->mesh->numVertices / 3);
			ImGui::Text("Vertices count: %d", App->scene->selectedGO->componentMesh->mesh->numVertices);
		}
	}

	if (App->scene->selectedGO->componentMaterial != NULL)
	{
		if (ImGui::CollapsingHeader("Material"))
		{
			if (App->scene->selectedGO->componentMaterial->material->program == 0)
			{
				if (App->scene->selectedGO->componentMaterial->material->texture0 != 0)
				{
					App->scene->selectedGO->componentMaterial->material;
					ImGui::Image((ImTextureID)App->scene->selectedGO->componentMaterial->material->texture0, ImVec2(200, 200));
					ImGui::Text("Dimensions: %dx%d", App->scene->selectedGO->componentMaterial->material->width, App->scene->selectedGO->componentMaterial->material->height);
				}
			}
			else if (App->scene->selectedGO->componentMaterial->material->program != 0)
			{

				const char* items[] = { "Flat", "Gouraud", "Phong", "Blinn" };
				int number = max(0, App->scene->selectedGO->componentMaterial->material->program - 2);
				if (ImGui::Combo("Shading", &number, items, IM_ARRAYSIZE(items)))
				{
					App->scene->selectedGO->componentMaterial->material->program = number + 2;
				}
				ImVec4 color = ImColor(App->scene->selectedGO->componentMaterial->material->color.x, App->scene->selectedGO->componentMaterial->material->color.y, App->scene->selectedGO->componentMaterial->material->color.z, App->scene->selectedGO->componentMaterial->material->color.w);
				if (ImGui::ColorEdit4("Color", (float*)&color))
				{
					App->scene->selectedGO->componentMaterial->material->color = math::float4(color.x, color.y, color.z, color.w);
				}
				ImGui::SliderFloat("Shininess", &App->scene->selectedGO->componentMaterial->material->shininess, 0, 128);
				ImGui::SliderFloat("K ambient", &App->scene->selectedGO->componentMaterial->material->k_ambient, 0, 1);
				ImGui::SliderFloat("K diffuse", &App->scene->selectedGO->componentMaterial->material->k_diffuse, 0, 1);
				ImGui::SliderFloat("K specular", &App->scene->selectedGO->componentMaterial->material->k_specular, 0, 1);
			}
		}
	}

	if (App->scene->selectedGO->componentCamera != NULL)
	{
		if (ImGui::CollapsingHeader("Camera"))
		{
			ImGui::PushItemWidth(75);
			ImGui::Text("Position:");
			ImGui::Text("X:");
			ImGui::SameLine();
			ImGui::PushID("1");
			ImGui::InputFloat("", &App->scene->selectedGO->componentCamera->frustum.pos.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Y:");
			ImGui::SameLine();
			ImGui::PushID("2");
			ImGui::InputFloat("", &App->scene->selectedGO->componentCamera->frustum.pos.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Z:");
			ImGui::SameLine();
			ImGui::PushID("3");
			ImGui::InputFloat("", &App->scene->selectedGO->componentCamera->frustum.pos.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopID();
			ImGui::Text("Front:");
			ImGui::Text("X:");
			ImGui::SameLine();
			ImGui::PushID("4");
			ImGui::InputFloat("", &App->scene->selectedGO->componentCamera->frustum.front.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Y:");
			ImGui::SameLine();
			ImGui::PushID("5");
			ImGui::InputFloat("", &App->scene->selectedGO->componentCamera->frustum.front.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Z:");
			ImGui::SameLine();
			ImGui::PushID("6");
			ImGui::InputFloat("", &App->scene->selectedGO->componentCamera->frustum.front.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopID();
			ImGui::Text("Up:");
			ImGui::Text("X:");
			ImGui::SameLine();
			ImGui::PushID("7");
			ImGui::InputFloat("", &App->scene->selectedGO->componentCamera->frustum.up.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Y:");
			ImGui::SameLine();
			ImGui::PushID("8");
			ImGui::InputFloat("", &App->scene->selectedGO->componentCamera->frustum.up.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Z:");
			ImGui::SameLine();
			ImGui::PushID("9");
			ImGui::InputFloat("", &App->scene->selectedGO->componentCamera->frustum.up.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopID();
			ImGui::PopItemWidth();
			ImGui::InputFloat("Movement Speed", &App->scene->selectedGO->componentCamera->mSpeed);
			ImGui::InputFloat("Rotation Speed", &App->scene->selectedGO->componentCamera->rSpeed);
			ImGui::InputFloat("Pitch", &App->scene->selectedGO->componentCamera->pitch);
			ImGui::InputFloat("Yaw", &App->scene->selectedGO->componentCamera->yaw);
			ImGui::InputFloat("Near Plane", &App->scene->selectedGO->componentCamera->frustum.nearPlaneDistance);
			ImGui::InputFloat("Far Plane", &App->scene->selectedGO->componentCamera->frustum.farPlaneDistance);
		}
	}
	ImGui::End();
}
