#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "GL/glew.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"
#include "SDL/include/SDL.h"


ModuleTextures::ModuleTextures()
{
}

// Destructor
ModuleTextures::~ModuleTextures()
{
	
}

// Called before render is available
bool ModuleTextures::Init()
{
	//ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	//ilutRenderer(ILUT_OPENGL);

	return true;
}

// Called before quitting
bool ModuleTextures::CleanUp()
{
	return true;
}

// Load new texture from file path
GLuint ModuleTextures::Load(const char* path)
{
	GLuint texture;

	ILuint image;

	ilGenImages(1, &image);

	ilBindImage(image);

	bool success = ilLoadImage(path);

	if (success)
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		glGenTextures(1, &texture);

		glBindTexture(GL_TEXTURE_2D, texture);

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// Set texture interpolation method to use linear interpolation (no MIPMAPS)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


		glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
			0,				// Pyramid level (for mip-mapping) - 0 is the top level
			ilGetInteger(IL_IMAGE_FORMAT),	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
			ilGetInteger(IL_IMAGE_WIDTH),	// Image width
			ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
			0,				// Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
			GL_UNSIGNED_BYTE,		// Image data type
			ilGetData());			// The actual image data itself

	}

	ilDeleteImage(image);

	return texture;
}

void ModuleTextures::Unload(unsigned id)
{
	if (id != 0)
	{
		glDeleteTextures(1, &id);
	}
}

