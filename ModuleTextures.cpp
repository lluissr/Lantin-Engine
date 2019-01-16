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

	if (texturesLoaded.count(path) > 0)
	{
		++texturesLoaded[path];
		return texturesInfo[path].info[0];
	}

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
		ILinfo lastImageInfo;
		iluGetImageInfo(&lastImageInfo);
		if (lastImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		glGenTextures(1, &texture);

		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH),
			ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

		if (texturesLoaded.count(path) > 0)
		{
			++texturesLoaded[path];
		}
		else
		{
			texturesLoaded[path] = 1;
			TextureInfo info = { texture, lastImageInfo.Width, lastImageInfo.Height };
			texturesInfo.insert(std::pair<std::string, TextureInfo>(path, info));
		}
	}
	else
	{
		LOG("Fail to load texture. Texture not found.")
	}

	ilDeleteImage(image);
	RELEASE_ARRAY(fileBuffer);

	return texture;
}

void ModuleTextures::Unload(unsigned id, const char* name)
{
	if (texturesLoaded.count(name) > 0)
	{
		if (texturesLoaded[name] == 1)
		{
			if (id != 0)
			{
				LOG("Unloading image %s", name);
				glDeleteTextures(1, &id);
				texturesLoaded.erase(name);
				texturesInfo.erase(name);
			}
		}
		else
		{
			--texturesLoaded[name];
		}
	}
}

unsigned ModuleTextures::GetTextureWidth(const char* name)
{
	assert(name != NULL);
	return texturesInfo[name].info[1];
}

unsigned ModuleTextures::GetTextureHeight(const char* name)
{
	assert(name != NULL);
	return texturesInfo[name].info[2];
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

				RELEASE_ARRAY(data)
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

