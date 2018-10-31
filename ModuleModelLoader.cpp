#include "Globals.h"
#include "Application.h"
#include "ModuleModelLoader.h"
#include "GL/glew.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
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
	const aiScene* scene = aiImportFile("BakerHouse.fbx", aiProcess_Triangulate);

	if (scene == NULL) {
		const char* a = aiGetErrorString();
	}

	for (unsigned i = 0; i < scene->mNumMeshes; ++i)
	{
		GenerateMeshData(scene->mMeshes[i]);
	}

	/*
	for (unsigned i = 0; i < scene->mNumMaterials; ++i)
	{
		GenerateMaterialData(scene->mMaterials[i]);
	}
	*/

	return true;
}

bool ModuleModelLoader::CleanUp()
{
	return true;
}

void ModuleModelLoader::GenerateMeshData(aiMesh* mesh)
{

	unsigned vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * 3 + sizeof(float) * 2)*mesh->mNumVertices, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * mesh->mNumVertices, mesh->mVertices);

	math::float2* coords = (math::float2*)glMapBufferRange(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices,
		sizeof(float) * 2 * mesh->mNumVertices, GL_MAP_WRITE_BIT);
	for (unsigned i = 0; i < mesh->mNumVertices; ++i)
	{
		coords[i] = math::float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
