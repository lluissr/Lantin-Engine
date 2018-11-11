#include "Globals.h"
#include "Application.h"
#include "ModuleModelLoader.h"
#include "ModuleTextures.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"
#include "GL/glew.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "MathGeoLib.h"
#include "SDL/include/SDL.h"


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

void ModuleModelLoader::ChooseModelToRender(const int num)
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


	aiVector3D translation;
	aiVector3D scaling;
	aiQuaternion rotation;

	aiNode* node = scene->mRootNode;
	node->mTransformation.Decompose(scaling, rotation, translation);
	float3 pos = { translation.x, translation.y, translation.z };
	float3 scale = { scaling.x, scaling.y, scaling.z };
	Quat rot = Quat(rotation.x, rotation.y, rotation.z, rotation.w);
	
	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i].translation = translation;
		meshes[i].scaling = scaling;
		meshes[i].rotation = rotation;
	}

	//std::string name = node->mName.C_Str();
	//static const char* transformNodes[5] = {
	//	"$AssimpFbx$_PreRotation", "$AssimpFbx$_Rotation", "$AssimpFbx$_PostRotation",
	//	"$AssimpFbx$_Scaling", "$AssimpFbx$_Translation" };

	//for (int i = 0; i < 5; i++)
	//{
	//	if (name.find(transformNodes[i]) != std::string::npos && node->mNumChildren > 0)
	//	{
	//		node = node->mChildren[0];

	//		node->mTransformation.Decompose(scaling, rotation, translation);

	//		pos += { translation.x, translation.y, translation.z };
	//		scale = { scale.x*scaling.x, scale.y*scaling.y, scale.z*scaling.z };
	//		rot = rot * Quat(rotation.x, rotation.y, rotation.z, rotation.w);

	//		name = node->mName.C_Str();
	//		i = -1;
	//	}
	//}

	App->camera->Focus();
}

bool ModuleModelLoader::CleanUp()
{
	CleanModel();

	return true;
}

void ModuleModelLoader::CleanModel()
{
	LOG("Cleaning meshes");
	for (unsigned i = 0; i < meshes.size(); ++i)
	{
		if (meshes[i].vbo != 0)
		{
			glDeleteBuffers(1, &meshes[i].vbo);
		}

		if (meshes[i].ibo != 0)
		{
			glDeleteBuffers(1, &meshes[i].ibo);
		}
	}
	meshes.clear();

	LOG("Cleaning materials");
	for (unsigned i = 0; i < materials.size(); ++i)
	{
		if (materials[i].texture0 != 0)
		{
			App->textures->Unload(materials[i].texture0);
		}
	}
	materials.clear();
}

void ModuleModelLoader::GenerateMeshData(const aiMesh* aiMesh)
{
	assert(aiMesh != NULL);

	Mesh mesh;

	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

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

	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

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

	mesh.material = aiMesh->mMaterialIndex;
	mesh.numVertices = aiMesh->mNumVertices;
	mesh.numIndices = aiMesh->mNumFaces * 3;
	mesh.name = aiMesh->mName.C_Str();



	meshes.push_back(mesh);
}

void ModuleModelLoader::GenerateMaterialData(const aiMaterial* aiMaterial)
{
	assert(aiMaterial != NULL);

	Material material;

	aiString file;
	aiTextureMapping mapping;
	unsigned uvindex = 0;

	if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file, &mapping, &uvindex) == AI_SUCCESS)
	{
		material.texture0 = App->textures->Load(file.data);
		material.width = App->textures->lastImageInfo.Width;
		material.height = App->textures->lastImageInfo.Height;
	}

	materials.push_back(material);
}

void ModuleModelLoader::ReplaceMaterial(const char* path)
{
	assert(path != NULL);

	Material material;
	material.texture0 = App->textures->Load(path);
	material.width = App->textures->lastImageInfo.Width;
	material.height = App->textures->lastImageInfo.Height;

	if (materials.size() == 0)
	{
		materials.push_back(material);
	}
	else
	{
		App->textures->Unload(materials[0].texture0);
		materials[0] = material;
	}
}

void ModuleModelLoader::DrawImGui()
{
	ImGui::Text("Model loaded has %d meshes", meshes.size());

	for (size_t i = 0; i < meshes.size(); i++)
	{

		ImGui::NewLine();
		ImGui::Text("Mesh name: %s", meshes[i].name);


		if (ImGui::TreeNode((void*)(i * 3), "Transformation"))
		{
			float pos[3] = { meshes[i].translation.x,meshes[i].translation.y, meshes[i].translation.z };
			float scaling[3] = { meshes[i].scaling.x,meshes[i].scaling.y, meshes[i].scaling.z };
			float rotation[3] = { meshes[i].rotation.x,meshes[i].rotation.y, meshes[i].rotation.z };
			ImGui::Text("Position:");
			ImGui::InputFloat3("", pos, 5, ImGuiInputTextFlags_ReadOnly);
			ImGui::Text("Rotation:");
			ImGui::InputFloat3("", rotation, 5, ImGuiInputTextFlags_ReadOnly);
			ImGui::Text("Scale:");
			ImGui::InputFloat3("", scaling, 5, ImGuiInputTextFlags_ReadOnly);
			ImGui::TreePop();

		}
		if (ImGui::TreeNode((void*)(i * 3 + 1), "Geometry"))
		{
			ImGui::Text("Triangles count: %d", meshes[i].numVertices / 3);
			ImGui::Text("Vertices count: %d", meshes[i].numVertices);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode((void*)(i * 3 + 2), "Textures"))
		{
			if (materials[meshes[i].material].texture0 != 0)
			{
				ImGui::Image((ImTextureID)materials[meshes[i].material].texture0, ImVec2(200, 200));
				ImGui::Text("Dimensions: %dx%d", materials[meshes[i].material].width, materials[meshes[i].material].height);
			}
			ImGui::TreePop();
		}
	}
}
