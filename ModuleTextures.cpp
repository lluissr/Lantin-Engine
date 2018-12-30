#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "GL/glew.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"
#include "SDL/include/SDL.h"

ModuleTextures::ModuleTextures()
{
}

ModuleTextures::~ModuleTextures()
{
}

bool ModuleTextures::Init()
{
	ilInit();
	iluInit();
	ilutInit();

	return true;
}

bool ModuleTextures::CleanUp()
{
	return true;
}

GLuint ModuleTextures::Load(const char* path)
{
	assert(path != NULL);

	GLuint texture = 0;

	ILuint image;

	ilGenImages(1, &image);

	ilBindImage(image);

	bool success = false;

	LOG("Try loading texture from path: %s", path);
	char* fileBuffer;
	unsigned lenghtBuffer = App->fileSystem->ReadFile(path, &fileBuffer);
	success = ilLoadL(IL_DDS, fileBuffer, lenghtBuffer);
	
	if (!success)
	{
		LOG("Fail at loading texture");
		std::string str(path);
		std::stringstream ss(str);
		std::string token;
		std::vector<std::string> cont;
		while (std::getline(ss, token, '\\')) {
			cont.push_back(token);
		}
		std::ostringstream stringStream;
		stringStream << "Library/Textures/" << cont[cont.size() - 1];

		LOG("2nd try for loading texture from path: %s", stringStream.str().c_str());
		lenghtBuffer = App->fileSystem->ReadFile(stringStream.str().c_str(), &fileBuffer);
		success = ilLoadL(IL_DDS, fileBuffer, lenghtBuffer);
		if (!success)
		{
			LOG("Fail at loading texture in second try");

			LOG("3rd tryfor loading texture from path: %s", cont[cont.size() - 1]);
			lenghtBuffer = App->fileSystem->ReadFile(cont[cont.size() - 1].c_str(), &fileBuffer);
			success = ilLoadL(IL_DDS, fileBuffer, lenghtBuffer);
		}
	}

	if (success)
	{
		LOG("Texture loaded correctly \n");
		iluGetImageInfo(&lastImageInfo);
		if (lastImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
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
	else
	{
		LOG("Fail to load texture. Texture not found.")
	}

	ilDeleteImage(image);

	return texture;
}

void ModuleTextures::Unload(unsigned id) const
{
	if (id != 0)
	{
		glDeleteTextures(1, &id);
	}
}


bool ModuleTextures::Import(const char* path)
{
	bool result = false;

	char* fileBuffer;
	unsigned lenghBuffer = App->fileSystem->ReadFile(path, &fileBuffer);

	if (fileBuffer)
	{
		ILuint ImageName;
		ilGenImages(1, &ImageName);
		ilBindImage(ImageName);

		if (ilLoadL(IL_TYPE_UNKNOWN, (const void*)fileBuffer, lenghBuffer))
		{
			ilEnable(IL_FILE_OVERWRITE);

			ILuint   size;
			ILubyte *data;
			ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
			size = ilSaveL(IL_DDS, NULL, 0);
			if (size > 0)
			{
				data = new ILubyte[size];
				if (ilSaveL(IL_DDS, data, size) > 0)
				{
					std::string fileName;
					std::string fileExtension;
					App->fileSystem->SplitPath(path, nullptr, &fileName, &fileExtension);

					fileName.insert(0, "/Library/Textures/");
					fileName.append(".dds");

					result = App->fileSystem->WriteFile(fileName.c_str(), data, size, false);
				}

				delete[] data;
				data = nullptr;
			}
			ilDeleteImages(1, &ImageName);
		}
	}

	if (result == false)
	{
		LOG("Cannot load texture from buffer of size %u", lenghBuffer);
	}

	return result;
}

