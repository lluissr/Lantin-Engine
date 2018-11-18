#ifndef __ModuleModelLoader_h__
#define __ModuleModelLoader_h__

#include "Module.h"
#include "Globals.h"
#include <assimp/mesh.h>
#include <assimp/material.h>
#include "GL/glew.h"
#include "MathGeoLib.h"
#include <vector>
#include "GameObject.h"


class Mesh
{
public:
	unsigned vbo = 0;
	unsigned ibo = 0;
	unsigned material = 0;
	unsigned numVertices = 0;
	unsigned numIndices = 0;
	const char* name = nullptr;
	aiVector3D translation;
	aiVector3D scaling;
	aiQuaternion rotation;
};

class Material
{
public:
	unsigned texture0 = 0;
	unsigned width = 0;
	unsigned height = 0;
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
	void ReplaceMaterial(const char* path);
	void DrawImGui();

	math::float3 minPoint;
	math::float3 maxPoint;


	//std::vector<Mesh*> meshes;
	std::vector<Material*> materials;

	int modelRendered = -1;
};

#endif
