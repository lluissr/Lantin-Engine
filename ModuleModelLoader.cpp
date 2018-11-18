#include "Globals.h"
#include "Application.h"
#include "ModuleModelLoader.h"
#include "ModuleTextures.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "GL/glew.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "MathGeoLib.h"
#include "SDL/include/SDL.h"
#include "ComponentMesh.h"


ModuleModelLoader::ModuleModelLoader()
{

}

// Destructor
ModuleModelLoader::~ModuleModelLoader()
{

}

bool ModuleModelLoader::Init()
{
	ChooseModelToRender(0);

	return true;
}

void ModuleModelLoader::ChooseModelToRender(int num)
{
	if (num == modelRendered) { return; }

	char* path = "";
	switch (num)
	{
	case 0:
		path = "BakerHouse.fbx";
		break;
	case 1:
		path = "Trex.fbx";
		break;
	case 2:
		path = "Radioactive_barrel.fbx";
		break;
	default:
		modelRendered = -1;
		return;
	}

	modelRendered = num;

	ImportModel(path);
}

void ModuleModelLoader::ImportModel(const char* path)
{
	assert(path != NULL);

	LOG("Try importing model from path: %s", path);
	const aiScene* scene = aiImportFile(path, aiProcess_Triangulate);

	if (scene == NULL)
	{
		const char* a = aiGetErrorString();
		LOG("Importing error: %s", a);
		return;
	}
	else
	{
		LOG("Fbx imported: %s", path);
	}


	if (scene->mNumMeshes > 0)
	{
		minPoint = math::float3(scene->mMeshes[0]->mVertices->x, scene->mMeshes[0]->mVertices->y, scene->mMeshes[0]->mVertices->z);
		maxPoint = math::float3(scene->mMeshes[0]->mVertices->x, scene->mMeshes[0]->mVertices->y, scene->mMeshes[0]->mVertices->z);
	}

	App->scene->CleanRootGameObjects();

	LOG("Start GenerateMeshData");
	for (unsigned i = 0; i < scene->mNumMeshes; ++i)
	{
		GenerateMeshData(scene->mMeshes[i]);
	}

	LOG("Start GenerateMaterialData");
	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		GenerateMaterialData(scene->mMaterials[i]);
	}

	for (GameObject* go : App->scene->root->gameObjects)
	{
		ComponentMaterial* component = (ComponentMaterial*)go->CreateComponent(ComponentType::MATERIAL);
		component->material = materials[go->mesh->mesh->material];
	}

	aiVector3D translation;
	aiVector3D scaling;
	aiQuaternion rotation;

	aiNode* node = scene->mRootNode;
	node->mTransformation.Decompose(scaling, rotation, translation);
	math::float3 pos = { translation.x, translation.y, translation.z };
	math::float3 scale = { scaling.x, scaling.y, scaling.z };
	math::Quat rot = math::Quat(rotation.x, rotation.y, rotation.z, rotation.w);

	for (GameObject* go : App->scene->root->gameObjects)
	{
		go->mesh->mesh->translation = translation;
		go->mesh->mesh->scaling = scaling;
		go->mesh->mesh->rotation = rotation;
	}

	App->camera->Focus();
}

bool ModuleModelLoader::CleanUp()
{
	CleanModel();

	return true;
}

void ModuleModelLoader::CleanModel()
{

	LOG("Cleaning materials");
	for (unsigned i = 0; i < materials.size(); ++i)
	{
		if (materials[i]->texture0 != 0)
		{
			App->textures->Unload(materials[i]->texture0);
		}
	}
	materials.clear();
}

void ModuleModelLoader::GenerateMeshData(const aiMesh* aiMesh)
{
	assert(aiMesh != NULL);

	Mesh* mesh = new Mesh();
	GameObject* gameObject = new GameObject(App->scene->root);

	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

	glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * 3 + sizeof(float) * 2)*aiMesh->mNumVertices, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * aiMesh->mNumVertices, aiMesh->mVertices);

	math::float2* texture_coords = (math::float2*)glMapBufferRange(GL_ARRAY_BUFFER, sizeof(float) * 3 * aiMesh->mNumVertices, sizeof(float) * 2 * aiMesh->mNumVertices, GL_MAP_WRITE_BIT);

	for (unsigned i = 0; i < aiMesh->mNumVertices; ++i)
	{
		texture_coords[i] = math::float2(aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y);
		if (aiMesh->mVertices[i].x < minPoint.x) { minPoint.x = aiMesh->mVertices[i].x; }
		if (aiMesh->mVertices[i].y < minPoint.y) { minPoint.y = aiMesh->mVertices[i].y; }
		if (aiMesh->mVertices[i].z < minPoint.z) { minPoint.z = aiMesh->mVertices[i].z; }
		if (aiMesh->mVertices[i].x > maxPoint.x) { maxPoint.x = aiMesh->mVertices[i].x; }
		if (aiMesh->mVertices[i].y > maxPoint.y) { maxPoint.y = aiMesh->mVertices[i].y; }
		if (aiMesh->mVertices[i].z > maxPoint.z) { maxPoint.z = aiMesh->mVertices[i].z; }
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &mesh->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*aiMesh->mNumFaces * 3, nullptr, GL_STATIC_DRAW);

	unsigned* indices = (unsigned*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned)*aiMesh->mNumFaces * 3, GL_MAP_WRITE_BIT);

	for (unsigned i = 0; i < aiMesh->mNumFaces; ++i)
	{
		assert(aiMesh->mFaces[i].mNumIndices == 3);

		*(indices++) = aiMesh->mFaces[i].mIndices[0];
		*(indices++) = aiMesh->mFaces[i].mIndices[1];
		*(indices++) = aiMesh->mFaces[i].mIndices[2];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	mesh->material = aiMesh->mMaterialIndex;
	mesh->numVertices = aiMesh->mNumVertices;
	mesh->numIndices = aiMesh->mNumFaces * 3;
	mesh->name = aiMesh->mName.C_Str();

	ComponentMesh* component = (ComponentMesh*)gameObject->CreateComponent(ComponentType::MESH);
	component->mesh = mesh;
	gameObject->name = aiMesh->mName.C_Str();
	App->scene->root->gameObjects.push_back(gameObject);
}

void ModuleModelLoader::GenerateMaterialData(const aiMaterial* aiMaterial)
{
	assert(aiMaterial != NULL);

	Material* material = new Material();

	aiString file;
	aiTextureMapping mapping;
	unsigned uvindex = 0;

	if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, &uvindex) == AI_SUCCESS)
	{
		material->texture0 = App->textures->Load(file.data);
		material->width = App->textures->lastImageInfo.Width;
		material->height = App->textures->lastImageInfo.Height;
	}

	materials.push_back(material);
}

void ModuleModelLoader::ReplaceMaterial(const char* path)
{
	assert(path != NULL);

	Material* material = new Material();
	material->texture0 = App->textures->Load(path);
	material->width = App->textures->lastImageInfo.Width;
	material->height = App->textures->lastImageInfo.Height;

	if (materials.size() == 0)
	{
		materials.push_back(material);
	}
	else
	{
		App->textures->Unload(materials[0]->texture0);
		unsigned int id = materials[0]->texture0;
		for (GameObject* go : App->scene->root->gameObjects)
		{
			if (go->material->material->texture0 == id)
				go->material->material = material;
		}

		delete materials[0];
		materials[0] = material;
	}
}

void ModuleModelLoader::DrawImGui()
{
	ImGui::Text("Model loaded has %d meshes", App->scene->root->gameObjects.size());
	int count = 0;
	for (GameObject* go : App->scene->root->gameObjects)
	{

		ImGui::NewLine();
		ImGui::Text("Mesh name: %s", go->name);


		if (ImGui::TreeNode((void*)(count * 3), "Transformation"))
		{
			float pos[3] = { go->mesh->mesh->translation.x, go->mesh->mesh->translation.y,  go->mesh->mesh->translation.z };
			float scaling[3] = { go->mesh->mesh->scaling.x, go->mesh->mesh->scaling.y,  go->mesh->mesh->scaling.z };
			float rotation[3] = { go->mesh->mesh->rotation.x, go->mesh->mesh->rotation.y,  go->mesh->mesh->rotation.z };
			ImGui::Text("Position:");
			ImGui::InputFloat3("", pos, 5, ImGuiInputTextFlags_ReadOnly);
			ImGui::Text("Rotation:");
			ImGui::InputFloat3("", rotation, 5, ImGuiInputTextFlags_ReadOnly);
			ImGui::Text("Scale:");
			ImGui::InputFloat3("", scaling, 5, ImGuiInputTextFlags_ReadOnly);
			ImGui::TreePop();

		}
		if (ImGui::TreeNode((void*)(count * 3 + 1), "Geometry"))
		{
			ImGui::Text("Triangles count: %d", go->mesh->mesh->numVertices / 3);
			ImGui::Text("Vertices count: %d", go->mesh->mesh->numVertices);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode((void*)(count * 3 + 2), "Textures"))
		{
			if (go->material->material->texture0 != 0)
			{
				ImGui::Image((ImTextureID)go->material->material->texture0, ImVec2(200, 200));
				ImGui::Text("Dimensions: %dx%d", go->material->material->width, go->material->material->height);
			}
			ImGui::TreePop();
		}
		++count;
	}
}
