#include "PanelModel.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ModuleModelLoader.h"
#include "ModuleFileSystem.h"
#include "ModuleTextures.h"

PanelModel::PanelModel()
{
	show = true;
}


PanelModel::~PanelModel()
{
	go = nullptr;
}

void PanelModel::Draw()
{
	ImGui::Begin("Model selected", &show);

	if (App->scene->selectedGO == nullptr)
	{
		ImGui::End();
		return;
	}

	go = App->scene->selectedGO;

	ImGui::Text("Name:");
	ImGui::SameLine();
	ImGui::InputText("##", &go->name[0], 40);
	ImGui::Text("Model selected has %d childs.", go->gameObjects.size());
	ImGui::NewLine();
	ImGui::Checkbox("Active", &go->isActive);
	ImGui::Checkbox("Static", &go->isStatic);
	ImGui::NewLine();

	if (go->componentCamera != nullptr)
	{
		if (ImGui::Button("Use as game camera"))
		{
			App->scene->UseAsGameCamera(go);
		}
		ImGui::NewLine();
	}

	if (go->componentMesh == nullptr)
	{
		if (ImGui::Button("Add mesh"))
		{
			go->CreateComponent(ComponentType::MESH);
		}
	}

	bool changed = false;
	if (ImGui::CollapsingHeader("Transformation"))
	{
		if (ImGui::Button("Apply identity matrix"))
		{
			go->localMatrix = math::float4x4::identity;
			go->position = { 0.0f,0.0f,0.0f };
			go->scale = { 1.0f,1.0f,1.0f };
			go->rotation = { 0.0f,0.0f,0.0f,1.0f };
			App->scene->CalculateGlobalMatrix(go);
			go->UpdateBoundingBox();
		}
		ImGui::NewLine();
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

		if (changed)
		{
			go->localMatrix.Set(float4x4::FromTRS(go->position, go->rotation, go->scale));
			App->scene->CalculateGlobalMatrix(go);
			go->UpdateBoundingBox();
		}

	}

	if (go->componentMesh != nullptr)
	{
		if (ImGui::CollapsingHeader("Geometry"))
		{
			DrawComboBoxMeshes();
			if (go->componentMesh->mesh != nullptr)
			{
				ImGui::Checkbox("Wireframe", &go->componentMesh->mesh->useWireframe);
				ImGui::Text("Triangles count: %d", go->componentMesh->mesh->numVertices / 3);
				ImGui::Text("Vertices count: %d", go->componentMesh->mesh->numVertices);
			}
		}
	}

	if (go->componentMaterial != nullptr)
	{
		if (ImGui::CollapsingHeader("Material"))
		{

			if (ImGui::CollapsingHeader("Diffuse"))
			{
				ImGui::ColorEdit4("Diffuse color", (float*)&go->componentMaterial->material->diffuseColor);
				DrawComboBoxMaterials(MaterialType::DIFFUSE);
				go->componentMaterial->material;
				ImGui::Image((ImTextureID)go->componentMaterial->material->diffuseMap, ImVec2(200, 200));
				ImGui::Text("Dimensions: %dx%d", go->componentMaterial->material->diffuseWidth, go->componentMaterial->material->diffuseHeight);
				ImGui::SliderFloat("K diffuse", &go->componentMaterial->material->k_diffuse, 0, 1);
			}

			if (ImGui::CollapsingHeader("Ambient"))
			{
				DrawComboBoxMaterials(MaterialType::OCCLUSION);
				go->componentMaterial->material;
				ImGui::Image((ImTextureID)go->componentMaterial->material->occlusionMap, ImVec2(200, 200));
				ImGui::Text("Dimensions: %dx%d", go->componentMaterial->material->ambientWidth, go->componentMaterial->material->ambientHeight);
				ImGui::SliderFloat("K ambient", &go->componentMaterial->material->k_ambient, 0, 1);
			}
			if (ImGui::CollapsingHeader("Specular"))
			{
				ImGui::ColorEdit4("Specular color", (float*)&go->componentMaterial->material->specularColor);
				DrawComboBoxMaterials(MaterialType::SPECULAR);
				go->componentMaterial->material;
				ImGui::Image((ImTextureID)go->componentMaterial->material->specularMap, ImVec2(200, 200));
				ImGui::Text("Dimensions: %dx%d", go->componentMaterial->material->specularWidth, go->componentMaterial->material->specularHeight);
				ImGui::SliderFloat("K specular", &go->componentMaterial->material->k_specular, 0, 1);
				ImGui::SliderFloat("Shininess", &go->componentMaterial->material->shininess, 0, 128);
			}
			if (ImGui::CollapsingHeader("Emissive"))
			{
				ImGui::ColorEdit4("Emissive color", (float*)&go->componentMaterial->material->emissiveColor);
				DrawComboBoxMaterials(MaterialType::EMISSIVE);
				go->componentMaterial->material;
				ImGui::Image((ImTextureID)go->componentMaterial->material->emissiveMap, ImVec2(200, 200));
				ImGui::Text("Dimensions: %dx%d", go->componentMaterial->material->emissiveWidth, go->componentMaterial->material->emissiveHeight);
			}

			/*
				const char* items[] = { "Flat", "Gouraud", "Phong", "Blinn" };
				int number = max(0, go->componentMaterial->material->program - 2);
				if (ImGui::Combo("Shading", &number, items, IM_ARRAYSIZE(items)))
				{
					go->componentMaterial->material->program = number + 2;
				}
				*/
		}
	}

	if (go->componentCamera != nullptr)
	{
		if (ImGui::CollapsingHeader("Camera"))
		{
			ImGui::Checkbox("Draw frustum", &go->componentCamera->showFrustum);
			ImGui::PushItemWidth(75);
			ImGui::Text("Position:");
			ImGui::Text("X:");
			ImGui::SameLine();
			ImGui::PushID("1");
			ImGui::InputFloat("", &go->componentCamera->frustum.pos.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Y:");
			ImGui::SameLine();
			ImGui::PushID("2");
			ImGui::InputFloat("", &go->componentCamera->frustum.pos.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Z:");
			ImGui::SameLine();
			ImGui::PushID("3");
			ImGui::InputFloat("", &go->componentCamera->frustum.pos.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopID();
			ImGui::Text("Front:");
			ImGui::Text("X:");
			ImGui::SameLine();
			ImGui::PushID("4");
			ImGui::InputFloat("", &go->componentCamera->frustum.front.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Y:");
			ImGui::SameLine();
			ImGui::PushID("5");
			ImGui::InputFloat("", &go->componentCamera->frustum.front.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Z:");
			ImGui::SameLine();
			ImGui::PushID("6");
			ImGui::InputFloat("", &go->componentCamera->frustum.front.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopID();
			ImGui::Text("Up:");
			ImGui::Text("X:");
			ImGui::SameLine();
			ImGui::PushID("7");
			ImGui::InputFloat("", &go->componentCamera->frustum.up.x, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Y:");
			ImGui::SameLine();
			ImGui::PushID("8");
			ImGui::InputFloat("", &go->componentCamera->frustum.up.y, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			ImGui::PopID();
			ImGui::Text("Z:");
			ImGui::SameLine();
			ImGui::PushID("9");
			ImGui::InputFloat("", &go->componentCamera->frustum.up.z, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopID();
			ImGui::PopItemWidth();
			ImGui::InputFloat("Movement Speed", &go->componentCamera->mSpeed);
			ImGui::InputFloat("Rotation Speed", &go->componentCamera->rSpeed);
			ImGui::InputFloat("Pitch", &go->componentCamera->pitch);
			ImGui::InputFloat("Yaw", &go->componentCamera->yaw);
			ImGui::InputFloat("Near Plane", &go->componentCamera->frustum.nearPlaneDistance);
			ImGui::InputFloat("Far Plane", &go->componentCamera->frustum.farPlaneDistance);
		}
	}
	
	ImGui::End();
}

void PanelModel::DrawComboBoxMaterials(MaterialType type)
{
	const char* comboBoxSelected = "Select a Texture";
	const char* id = nullptr;
	switch (type)
	{
	case MaterialType::DIFFUSE:
		id = "Diffuse";
		comboBoxSelected = go->componentMaterial->material->diffuseMapName != nullptr ? go->componentMaterial->material->diffuseMapName : "Select a Texture";
		break;
	case MaterialType::OCCLUSION:
		id = "Occlusion";
		comboBoxSelected = go->componentMaterial->material->occlusionMapName != nullptr ? go->componentMaterial->material->occlusionMapName : "Select a Texture";
		break;
	case MaterialType::SPECULAR:
		id = "Specular";
		comboBoxSelected = go->componentMaterial->material->specularMapName != nullptr ? go->componentMaterial->material->specularMapName : "Select a Texture";
		break;
	case MaterialType::EMISSIVE:
		id = "Emissive";
		comboBoxSelected = go->componentMaterial->material->emissiveMapName != nullptr ? go->componentMaterial->material->emissiveMapName : "Select a Texture";
		break;
	}

	if (App->fileSystem->texturesList.size() > 0)
	{
		ImGui::PushID(id);
		if (ImGui::BeginCombo("##", comboBoxSelected))
		{
			for (std::vector<std::string>::iterator iterator = App->fileSystem->texturesList.begin(); iterator != App->fileSystem->texturesList.end(); ++iterator)
			{
				bool isSelected = (comboBoxSelected == (*iterator).c_str());
				if (ImGui::Selectable((*iterator).c_str(), isSelected))
				{
					comboBoxSelected = (*iterator).c_str();
					switch (type)
					{
					case MaterialType::DIFFUSE:
						App->textures->Unload(go->componentMaterial->material->diffuseMap);
						go->componentMaterial->material->diffuseMap = App->textures->Load(comboBoxSelected);
						go->componentMaterial->material->diffuseWidth = App->textures->lastImageInfo.Width;
						go->componentMaterial->material->diffuseHeight = App->textures->lastImageInfo.Height;
						go->componentMaterial->material->diffuseMapName = comboBoxSelected;
						break;
					case MaterialType::OCCLUSION:
						App->textures->Unload(go->componentMaterial->material->occlusionMap);
						go->componentMaterial->material->occlusionMap = App->textures->Load(comboBoxSelected);
						go->componentMaterial->material->ambientWidth = App->textures->lastImageInfo.Width;
						go->componentMaterial->material->ambientHeight = App->textures->lastImageInfo.Height;
						go->componentMaterial->material->occlusionMapName = comboBoxSelected;
						break;
					case MaterialType::SPECULAR:
						App->textures->Unload(go->componentMaterial->material->specularMap);
						go->componentMaterial->material->specularMap = App->textures->Load(comboBoxSelected);
						go->componentMaterial->material->specularWidth = App->textures->lastImageInfo.Width;
						go->componentMaterial->material->specularHeight = App->textures->lastImageInfo.Height;
						go->componentMaterial->material->specularMapName = comboBoxSelected;
						break;
					case MaterialType::EMISSIVE:
						App->textures->Unload(go->componentMaterial->material->emissiveMap);
						go->componentMaterial->material->emissiveMap = App->textures->Load(comboBoxSelected);
						go->componentMaterial->material->emissiveWidth = App->textures->lastImageInfo.Width;
						go->componentMaterial->material->emissiveHeight = App->textures->lastImageInfo.Height;
						go->componentMaterial->material->emissiveMapName = comboBoxSelected;
						break;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopID();
	}
}

void PanelModel::DrawComboBoxMeshes()
{
	const char* comboBoxSelected = (go->componentMesh->mesh != nullptr && go->componentMesh->mesh->meshName != nullptr) ? go->componentMesh->mesh->meshName : "Select a Mesh";

	if (App->fileSystem->meshList.size() > 0)
	{
		ImGui::PushID("Meshes");
		if (ImGui::BeginCombo("##", comboBoxSelected))
		{
			for (std::vector<std::string>::iterator iterator = App->fileSystem->meshList.begin(); iterator != App->fileSystem->meshList.end(); ++iterator)
			{
				bool isSelected = (comboBoxSelected == (*iterator).c_str());
				if (ImGui::Selectable((*iterator).c_str(), isSelected))
				{
					comboBoxSelected = (*iterator).c_str();
					Mesh* mesh = App->modelLoader->Load(comboBoxSelected);
					if (mesh != nullptr)
					{
						if (go->componentMesh->mesh != nullptr)
						{
							delete go->componentMesh->mesh;
						}
						go->componentMesh->mesh = mesh;
						Material* material = new Material();
						ComponentMaterial* cmaterial = (ComponentMaterial*)go->CreateComponent(ComponentType::MATERIAL);
						cmaterial->material = material;
						App->scene->CalculateGlobalMatrix(App->scene->root);
						go->UpdateBoundingBox();
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopID();
	}
}
