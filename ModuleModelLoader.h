#ifndef __ModuleModelLoader_h__
#define __ModuleModelLoader_h__

#include<list>
#include "Module.h"
#include "Globals.h"
#include <assimp/mesh.h>
#include "GL/glew.h"


class ModuleModelLoader : public Module
{
public:
	ModuleModelLoader();
	~ModuleModelLoader();

	bool Init();
	bool CleanUp();

	void GenerateMeshData(aiMesh* mesh);
};

#endif
