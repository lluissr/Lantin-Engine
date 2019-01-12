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
	if (PHYSFS_exists("/Library/Scene/") == 0)
		PHYSFS_mkdir("/Library/Scene/");

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

unsigned ModuleFileSystem::WriteFile(const char * pathAndFileName, const void * buffer, unsigned size, bool append)
{
	unsigned result = 0;

	bool overwrite = PHYSFS_exists(pathAndFileName) != 0;
	PHYSFS_file* fsFile = (append) ? PHYSFS_openAppend(pathAndFileName) : PHYSFS_openWrite(pathAndFileName);

	if (fsFile != nullptr)
	{
		unsigned written = (unsigned)PHYSFS_write(fsFile, (const void*)buffer, 1, size);
		if (written == size)
		{
			result = written;
		}
		else
		{
			LOG("Error while writing to file %s: %s", pathAndFileName, PHYSFS_getLastError());
		}

		if (PHYSFS_close(fsFile) == 0)
		{
			LOG("Error closing file %s: %s", pathAndFileName, PHYSFS_getLastError());
		}
	}
	else
	{
		LOG("Error opening file %s: %s", pathAndFileName, PHYSFS_getLastError());
	}

	return result;
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

void ModuleFileSystem::GetExtension(const char* full_path, std::string* extension)
{
	std::string str = NormalizePath(full_path);
	unsigned pos_dot = str.find_last_of('.');

	if (pos_dot < str.length())
		*extension = str.substr(pos_dot + 1);
	else
		extension->clear();
}

std::string ModuleFileSystem::NormalizePath(const char * path)
{
	std::string str = path;

	for (unsigned int i = 0; i < str.length(); i++)
	{
		if (str[i] == '\\')
			str[i] = '/';
	}

	return str.c_str();
}

void ModuleFileSystem::GetFilesFromDirectoryRecursive(const char * directory, std::map<std::string, std::string>& result)
{
	char **enumeratedFiles = PHYSFS_enumerateFiles(directory);
	char **iterator;

	std::string dir(directory);
	std::vector<std::string> directoryList;

	for (iterator = enumeratedFiles; *iterator != nullptr; iterator++)
	{
		if (PHYSFS_isDirectory((dir + *iterator).c_str()))
		{
			directoryList.push_back(*iterator);
		}
		else
		{
			result[(*iterator)] = dir;
		}
	}

	PHYSFS_freeList(enumeratedFiles);

	for (std::vector<std::string>::iterator iterator = directoryList.begin(); iterator != directoryList.end(); ++iterator)
	{
		(*iterator).insert(0, directory);
		(*iterator).append("/");
		GetFilesFromDirectoryRecursive((*iterator).c_str(), result);
	}
}


void ModuleFileSystem::GetFilesFromDirectory(const char * directory, std::vector<std::string>& fileList) const
{
	char **enumeratedFiles = PHYSFS_enumerateFiles(directory);
	char **iterator;

	std::string dir(directory);

	for (iterator = enumeratedFiles; *iterator != nullptr; iterator++)
	{
		if (!PHYSFS_isDirectory((dir + *iterator).c_str()))
		{
			fileList.push_back(*iterator);
		}
	}

	PHYSFS_freeList(enumeratedFiles);
}


void ModuleFileSystem::UpdateFiles()
{
	meshList.clear();
	texturesList.clear();
	GetFilesFromDirectory("/Library/Meshes/", meshList);
	GetFilesFromDirectory("/Library/Textures/", texturesList);
}

bool ModuleFileSystem::Remove(const char * pathAndFileName)
{
	bool result = false;

	if (pathAndFileName != nullptr)
	{
		PHYSFS_delete(pathAndFileName);
		LOG("File deleted: [%s]", pathAndFileName);
		result = true;
	}

	return result;
}
