#ifndef __ModuleTextures_h__
#define __ModuleTextures_h__

#include<list>
#include "Module.h"
#include "Globals.h"
#include "GL/glew.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"
#include <assert.h>
#include <map>

struct SDL_Texture;

struct TextureInfo
{
	unsigned info[3];
};

class ModuleTextures : public Module
{
public:
	ModuleTextures();
	~ModuleTextures();

	bool Init();
	bool CleanUp();

	GLuint Load(const char* path);
	void Unload(unsigned id, const char* name);
	unsigned GetTextureWidth(const char* name);
	unsigned GetTextureHeight(const char* name);

	bool Import(const char* path);

private:
	std::map<std::string, unsigned> texturesLoaded;
	std::map<std::string, TextureInfo> texturesInfo;

};

#endif