#include "ModuleFileSystem.h"
#include "physfs.h"
#include "SDL/include/SDL_filesystem.h"

ModuleFileSystem::ModuleFileSystem()
{
}


ModuleFileSystem::~ModuleFileSystem()
{
	PHYSFS_deinit();
}


bool ModuleFileSystem::Init()
{
	char* basePath = SDL_GetPrefPath("Lantin", "");
	PHYSFS_init(basePath);
	int e = PHYSFS_isInit();
	SDL_free(basePath);
	const char * g = PHYSFS_getLastError();
	AddPath(".");

	AddPath("./Game/");

	PHYSFS_setWriteDir(".");

	if (PHYSFS_exists("/Library/") == 0)
		PHYSFS_mkdir("/Library/");
	if (PHYSFS_exists("/Library/Textures/") == 0)
		PHYSFS_mkdir("/Library/Textures/");
	if (PHYSFS_exists("/Library/Meshes/") == 0)
		PHYSFS_mkdir("/Library/Meshes/");
	return true;
}

bool ModuleFileSystem::AddPath(const char * path)
{
	bool result = false;

	if (PHYSFS_mount(path, nullptr, 1) == 0)
	{
		LOG("Error adding a path: %s\n", PHYSFS_getLastError());
	}
	else
	{
		result = true;
	}

	return result;
}

bool ModuleFileSystem::FileExists(const char* path, const char* atDirectory, const char* withExtension)
{
	return false;
}

unsigned ModuleFileSystem::ReadFile(const char * pathAndFileName, char** buffer)
{
	unsigned result = 0;

	PHYSFS_file* fsFile = PHYSFS_openRead(pathAndFileName);

	if (fsFile != nullptr)
	{
		PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(fsFile);

		if (size > 0)
		{
			*buffer = new char[size];
			unsigned readed = (unsigned)PHYSFS_read(fsFile, *buffer, 1, size);
			if (readed != size)
			{
				LOG("Error reading from file %s: %s\n", pathAndFileName, PHYSFS_getLastError());
				delete buffer;
			}
			else
			{
				result = readed;
			}
		}

		if (PHYSFS_close(fsFile) == 0)
		{
			LOG("Error closing file %s: %s\n", pathAndFileName, PHYSFS_getLastError());
		}
	}
	else
	{
		const char* error = PHYSFS_getLastError();
		LOG("Error opening file %s: %s\n", pathAndFileName, error);
	}

	return result;
}

unsigned ModuleFileSystem::WriteFile(const char * path, const void * buffer, unsigned size, bool overwrite)
{
	if (!overwrite)
	{
		path = GetAvailablePath(path);
		LOG("File with same name already exists - Saved as: %s", path);
	}

	PHYSFS_file* file = PHYSFS_openWrite(path);
	if (file != nullptr)
	{
		unsigned written = PHYSFS_write(file, (const void*)buffer, 1, size);
		PHYSFS_close(file);

		if (written != size)
		{
			LOG("Error on writting file: Written data size does not match buffer size - PHYSFS: %s", PHYSFS_getLastError());
			return 0;
		}

		return written;
	}
	LOG("Error opening file for writting - PHYSFS: %d", PHYSFS_getLastError());
	return 0;
}

const char* ModuleFileSystem::GetAvailablePath(const char* path)
{
	unsigned num_version = 1;

	std::string directory;
	std::string filename;
	std::string extension;
	SplitPath(path, &directory, &filename, &extension);

	std::string full_path = path;
	while (FileExists(full_path.c_str()))
	{
		num_version++;
		full_path = directory + filename + '(' + std::to_string(num_version) + ')' + extension;
	}

	return full_path.c_str();
}


void ModuleFileSystem::SplitPath(const char* full_path, std::string* path, std::string* filename, std::string* extension)
{
	std::string str = NormalizePath(full_path);
	unsigned pos_slash = str.find_last_of('/');
	unsigned pos_dot = str.find_last_of('.');

	if (path != nullptr)
	{
		if (pos_slash < str.length())
			*path = str.substr(0, pos_slash + 1);
		else
			path->clear();
	}
	if (filename != nullptr)
	{
		if (pos_slash < str.length())
			*filename = str.substr(pos_slash + 1, pos_dot - pos_slash - 1);
		else
			*filename = str.substr(0, pos_dot);
	}
	if (extension != nullptr)
	{
		if (pos_dot < str.length())
			*extension = str.substr(pos_dot + 1);
		else
			extension->clear();
	}
}

std::string ModuleFileSystem::NormalizePath(const char * path)
{
	std::string str = path;

	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == '\\')
			str[i] = '/';
	}

	return str.c_str();
}

