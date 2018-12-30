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
#include <string>

struct aiScene;
struct aiNode;
struct par_shapes_mesh_s;

class Mesh
{
public:
	unsigned vbo = 0;
	unsigned ibo = 0;
	unsigned material = 0;
	unsigned numVertices = 0;
	unsigned numIndices = 0;
	unsigned normalsOffset = 0;
	unsigned vertexSize = 0;
	unsigned vao = 0;
	unsigned texturesOffset = 0;
	math::AABB localBoundingBox;
	math::AABB globalBoundingBox;
	bool useWireframe = false;

	float* normals = nullptr;
	unsigned* indices = nullptr;
	float* vertices = nullptr;
	float* texCoords = nullptr;
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

	void CleanModel();
	
	bool LoadSphere(const char* name, float size, unsigned slices, unsigned stacks, const math::float4& color);
	bool LoadTorus(const char* name, float innerRadius, float outerRadius, unsigned slices, unsigned stacks, const math::float4& color);
	Mesh* CreateMeshFromParShapes(par_shapes_mesh_s* mesh);
	
	bool Import(const char* path);
	bool ImportMesh(const aiMesh* aiMesh, const char* name);
	bool SaveMesh(Mesh* mesh, std::string& newpath);
	Mesh* Load(const char* path);
	void GenerateVBO(Mesh& mesh);
	void GenerateVAO(Mesh& mesh);

	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
};

#endif
