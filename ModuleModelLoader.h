#ifndef __ModuleModelLoader_h__
#define __ModuleModelLoader_h__

#include "Application.h"
#include "Module.h"
#include "Globals.h"
#include <assimp/mesh.h>
#include <assimp/material.h>
#include "GL/glew.h"
#include "MathGeoLib.h"
#include <vector>
#include "GameObject.h"

struct aiScene;
struct aiNode;

class Mesh
{
public:
	unsigned vbo = 0;
	unsigned ibo = 0;
	unsigned material = 0;
	unsigned numVertices = 0;
	unsigned numIndices = 0;
	unsigned normals_offset = 0;
	unsigned vertex_size = 0;
	unsigned vao = 0;
};

class Material
{
public:
	unsigned program = 0;
	unsigned texture0 = 0;
	unsigned width = 0;
	unsigned height = 0;
	math::float4 color = math::float4::zero;
	float shininess = 0.0f;
	float k_specular = 0.0f;
	float k_ambient = 0.0f;
	float k_diffuse = 0.0f;
};

class ModuleModelLoader : public Module
{
public:
	ModuleModelLoader();
	~ModuleModelLoader();

	bool Init();
	bool CleanUp();

	void ChooseModelToRender(int num);
	void ImportModel(const char* path);
	void CleanModel();

	void GenerateMeshData(const aiMesh* aiMesh);
	void GenerateMaterialData(const aiMaterial* aiMaterial);
	GameObject* CreateGameObjects(const aiScene * scene, aiNode* node);
	void ReplaceMaterial(const char* path);
	bool LoadSphere(const char* name, float size, unsigned slices, unsigned stacks, const math::float4& color);
	void GenerateVAO(Mesh& mesh);

	math::float3 minPoint;
	math::float3 maxPoint;


	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;

	int modelRendered = -1;
};

#endif
