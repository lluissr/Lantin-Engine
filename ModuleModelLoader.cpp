#include "Globals.h"
#include "Application.h"
#include "ModuleModelLoader.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "MathGeoLib.h"
#include "SDL/include/SDL.h"
#include "ComponentMesh.h"
#include "GL/glew.h"

#pragma warning(push)
#pragma warning(disable : 4996)  
#pragma warning(disable : 4244)  
#pragma warning(disable : 4305)  
#pragma warning(disable : 4838)  

#define PAR_SHAPES_IMPLEMENTATION
#include "par_shapes.h"
#pragma warning(pop)


ModuleModelLoader::ModuleModelLoader()
{

}

// Destructor
ModuleModelLoader::~ModuleModelLoader()
{

}

bool ModuleModelLoader::Init()
{
	//LoadSphere("Sphere1", 1.0f, 30, 30, float4(1.0f, 0.0f, 0.0f, 1.0f));
	//LoadTorus("Torus1", 0.5f, 0.67f, 30, 30, float4(0.0f, 1.0f, 0.0f, 1.0f));

	App->scene->CalculateGlobalMatrix(App->scene->root);

	return true;
}

bool ModuleModelLoader::CleanUp()
{
	LOG("Cleaning meshes");
	for (unsigned i = 0; i < meshes.size(); ++i)
	{
		if (meshes[i]->vbo != 0)
		{
			glDeleteBuffers(1, &meshes[i]->vbo);
		}

		if (meshes[i]->ibo != 0)
		{
			glDeleteBuffers(1, &meshes[i]->ibo);
		}
		delete meshes[i];
	}
	meshes.clear();

	LOG("Cleaning materials");
	for (unsigned i = 0; i < materials.size(); ++i)
	{
		if (materials[i]->texture0 != 0)
		{
			App->textures->Unload(materials[i]->texture0);
		}
		delete materials[i];
	}
	materials.clear();

	return true;
}

void ModuleModelLoader::CleanModel()
{
	meshes.clear();

	materials.clear();
}


bool ModuleModelLoader::LoadSphere(const char* name, float size, unsigned slices, unsigned stacks, const math::float4& color)
{
	par_shapes_mesh* parMesh = par_shapes_create_parametric_sphere(int(slices), int(stacks));

	if (parMesh)
	{
		par_shapes_scale(parMesh, size, size, size);

		GameObject* go = new GameObject();
		go->name = name;

		ComponentMesh* cmesh = (ComponentMesh*)go->CreateComponent(ComponentType::MESH);
		cmesh->mesh = CreateMeshFromParShapes(parMesh);

		GenerateVAO(*cmesh->mesh);

		Material* material = new Material();
		material->program = 5;
		material->color = color;
		material->shininess = 64.0f;
		material->k_ambient = 1.0f;
		material->k_diffuse = 0.5f;
		material->k_specular = 0.6f;
		ComponentMaterial* cmaterial = (ComponentMaterial*)go->CreateComponent(ComponentType::MATERIAL);
		cmaterial->material = material;

		go->parent = App->scene->root;
		go->UpdateBoundingBox();
		App->scene->root->gameObjects.push_back(go);

		par_shapes_free_mesh(parMesh);

		return true;
	}

	return false;
}

bool ModuleModelLoader::LoadTorus(const char* name, float innerRadius, float outerRadius, unsigned slices, unsigned stacks, const math::float4& color)
{
	par_shapes_mesh* parMesh = par_shapes_create_torus(int(slices), int(stacks), innerRadius);

	if (parMesh)
	{
		par_shapes_scale(parMesh, outerRadius, outerRadius, outerRadius);

		GameObject* go = new GameObject();
		go->name = name;

		ComponentMesh* cmesh = (ComponentMesh*)go->CreateComponent(ComponentType::MESH);
		cmesh->mesh = CreateMeshFromParShapes(parMesh);

		GenerateVAO(*cmesh->mesh);

		Material* material = new Material();
		material->program = 5;
		material->color = color;
		material->shininess = 64.0f;
		material->k_ambient = 1.0f;
		material->k_diffuse = 0.5f;
		material->k_specular = 0.6f;
		ComponentMaterial* cmaterial = (ComponentMaterial*)go->CreateComponent(ComponentType::MATERIAL);
		cmaterial->material = material;

		go->parent = App->scene->root;
		go->UpdateBoundingBox();
		App->scene->root->gameObjects.push_back(go);

		par_shapes_free_mesh(parMesh);

		return true;
	}

	return false;
}

Mesh* ModuleModelLoader::CreateMeshFromParShapes(par_shapes_mesh_s* parMesh)
{
	assert(parMesh != NULL);

	Mesh* mesh = new Mesh();

	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

	unsigned offset = sizeof(math::float3);

	if (parMesh->normals)
	{
		mesh->normalsOffset = offset;
		offset += sizeof(math::float3);
	}

	mesh->vertexSize = offset;

	glBufferData(GL_ARRAY_BUFFER, mesh->vertexSize*parMesh->npoints, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(math::float3)*parMesh->npoints, parMesh->points);

	if (parMesh->normals)
	{
		glBufferSubData(GL_ARRAY_BUFFER, mesh->normalsOffset*parMesh->npoints, sizeof(math::float3)*parMesh->npoints, parMesh->normals);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &mesh->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*parMesh->ntriangles * 3, nullptr, GL_STATIC_DRAW);

	unsigned* indices = (unsigned*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0,
		sizeof(unsigned)*parMesh->ntriangles * 3, GL_MAP_WRITE_BIT);

	for (unsigned i = 0; i< unsigned(parMesh->ntriangles * 3); ++i)
	{
		*(indices++) = parMesh->triangles[i];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	mesh->material = 0;
	mesh->numVertices = parMesh->npoints;
	mesh->numIndices = parMesh->ntriangles * 3;
	mesh->localBoundingBox.Enclose((float3*)parMesh->points, mesh->numVertices);

	return mesh;
}

void ModuleModelLoader::GenerateVAO(Mesh& mesh)
{
	glGenVertexArrays(1, &mesh.vao);

	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	if (mesh.normalsOffset != 0)
	{
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(mesh.normalsOffset * mesh.numVertices));
	}

	if (mesh.texturesOffset != 0)
	{
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(mesh.texturesOffset * mesh.numVertices));
	}

	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


bool ModuleModelLoader::Import(const char* path)
{
	bool result = false;

	char* buffer;
	unsigned lenghtBuffer = App->fileSystem->ReadFile(path, &buffer);
	const aiScene* scene = aiImportFileFromMemory(buffer, lenghtBuffer, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate, "");
	
	if (scene == NULL)
	{
		const char* a = aiGetErrorString();
		LOG("Importing error: %s", a);
		return false;
	}
	else
	{
		LOG("Fbx imported for export: %s", path);
	}
	std::string name;
	App->fileSystem->SplitPath(path, nullptr, &name, nullptr);
	for (unsigned i = 0; i < scene->mNumMeshes; ++i)
	{
		std::string meshName = name;
		meshName.append("_" + std::to_string(i));
		ImportMesh(scene->mMeshes[i], meshName.c_str());
	}

	return result;
}


bool ModuleModelLoader::ImportMesh(const aiMesh* mesh, const char* name)
{
	bool ret = false;
	Mesh* newMesh = new Mesh();

	if (mesh->mNumVertices > 0)
	{
		newMesh->numVertices = mesh->mNumVertices;

		newMesh->vertices = new float[newMesh->numVertices * 3];
		memcpy(newMesh->vertices, mesh->mVertices, sizeof(float)*newMesh->numVertices * 3);
	}

	if (mesh->HasNormals())
	{
		newMesh->normalsOffset = mesh->mNumVertices;
		newMesh->normals = new float[newMesh->normalsOffset * 3];
		memcpy(newMesh->normals, mesh->mNormals, sizeof(float)*newMesh->normalsOffset * 3);
	}

	if (mesh->HasFaces())
	{
		ret = true;
		int t = 0;
		if (mesh->HasTextureCoords(0))
		{
			newMesh->texturesOffset = mesh->mNumVertices;
			newMesh->texCoords = new float[newMesh->texturesOffset * 2];
			for (unsigned q = 0; q < newMesh->numVertices * 2; q = q + 2)
			{
				newMesh->texCoords[q] = mesh->mTextureCoords[0][t].x;
				newMesh->texCoords[q + 1] = mesh->mTextureCoords[0][t].y;
				t++;
			}
		}

		newMesh->numIndices = mesh->mNumFaces * 3;
		newMesh->indices = new unsigned[newMesh->numIndices];

		for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
		{
			if (mesh->mFaces[j].mNumIndices != 3)
			{
				LOG("WARNING, geometry face with != 3 indices!");
				LOG("WARNING, face normals couldn't be loaded");
				delete newMesh;
				ret = false;
				break;
			}
			else
			{
				memcpy(&newMesh->indices[j * 3], mesh->mFaces[j].mIndices, 3 * sizeof(unsigned));
			}
		}
		
	}
	else
	{
		LOG("Current mesh has no faces, so will not be loaded");
		ret = false;
	}

	if (ret) {
		std::string fileName = "Library/Meshes/";
		fileName += name;
		fileName += ".pisifai";
		SaveMesh(newMesh, fileName);
	}

	delete newMesh;

	return ret;
}


bool ModuleModelLoader::SaveMesh(Mesh* mesh, std::string& path)
{
	bool ret = true;

	unsigned ranges[4] = { mesh->numVertices, mesh->numIndices, mesh->normalsOffset, mesh->texturesOffset };

	unsigned size = sizeof(ranges) + sizeof(float)*mesh->numVertices * 3 + sizeof(unsigned)*mesh->numIndices + sizeof(float)*mesh->normalsOffset * 3 + sizeof(float)*mesh->texturesOffset * 2;
	char* buffer = new char[size];
	char* cursor = buffer;

	unsigned bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	bytes = sizeof(float)*mesh->numVertices * 3;
	memcpy(cursor, mesh->vertices, bytes);
	cursor += bytes;

	bytes = sizeof(unsigned)*mesh->numIndices;
	memcpy(cursor, mesh->indices, bytes);
	cursor += bytes;

	bytes = sizeof(float)*mesh->normalsOffset * 3;
	memcpy(cursor, mesh->normals, bytes);
	cursor += bytes;

	bytes = sizeof(float)*mesh->texturesOffset * 2;
	memcpy(cursor, mesh->texCoords, bytes);
	cursor += bytes;

	if (App->fileSystem->WriteFile(path.c_str(), buffer, size, false) == 0)
	{
		ret = false;
	}
	else
	{
		LOG("Mesh saved as %s", path.c_str());
	}
	delete[] buffer;

	return ret;
}


Mesh* ModuleModelLoader::Load(const char* path)
{
	Mesh* mesh = new Mesh;

	std::string pathName = "Library/Meshes/";
	pathName += path;
	char* buffer = nullptr;
	unsigned size = App->fileSystem->ReadFile(pathName.c_str(), &buffer);
	char* cursor = buffer;

	unsigned ranges[4];
	unsigned bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;

	mesh->numVertices = ranges[0];
	mesh->numIndices = ranges[1];
	mesh->normalsOffset = ranges[2];
	mesh->texturesOffset = ranges[3];

	if (mesh->numVertices > 0)
	{
		bytes = sizeof(float)*mesh->numVertices * 3;
		mesh->vertices = new float[mesh->numVertices * 3];
		memcpy(mesh->vertices, cursor, bytes);
		cursor += bytes;
	}

	if (mesh->numIndices > 0)
	{
		bytes = sizeof(unsigned)*mesh->numIndices;
		mesh->indices = new unsigned[mesh->numIndices];
		memcpy(mesh->indices, cursor, bytes);
		cursor += bytes;
	}

	if (mesh->normalsOffset > 0)
	{
		bytes = sizeof(float)*mesh->normalsOffset * 3;
		mesh->normals = new float[mesh->normalsOffset * 3];
		memcpy(mesh->normals, cursor, bytes);
		cursor += bytes;
	}

	if (mesh->texturesOffset > 0)
	{
		bytes = sizeof(float)*mesh->texturesOffset * 2;
		mesh->texCoords = new float[mesh->texturesOffset * 2];
		memcpy(mesh->texCoords, cursor, bytes);
		cursor += bytes;
	}

	delete[] buffer;
	
	GenerateVBO(*mesh);
	GenerateVAO(*mesh);
	mesh->localBoundingBox.SetNegativeInfinity();
	mesh->localBoundingBox.Enclose((float3*)mesh->vertices, mesh->numVertices);

	return mesh;
}

void ModuleModelLoader::GenerateVBO(Mesh& mesh) 
{
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

	unsigned offset = sizeof(math::float3);

	if (mesh.normals != nullptr)
	{
		mesh.normalsOffset = offset;
		offset += sizeof(math::float3);
	}

	if (mesh.texCoords != nullptr)
	{
		mesh.texturesOffset = offset;
		offset += sizeof(math::float2);
	}

	mesh.vertexSize = offset;

	glBufferData(GL_ARRAY_BUFFER, mesh.vertexSize*mesh.numVertices, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * mesh.numVertices, mesh.vertices);

	if (mesh.normals != nullptr)
	{
		glBufferSubData(GL_ARRAY_BUFFER, mesh.normalsOffset*mesh.numVertices, sizeof(float) * 3 * mesh.numVertices, mesh.normals);
	}

	if (mesh.texCoords != nullptr)
	{
		glBufferSubData(GL_ARRAY_BUFFER, mesh.texturesOffset * mesh.numVertices, sizeof(float2)*mesh.numVertices, mesh.texCoords);
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.numIndices * sizeof(unsigned), mesh.indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}