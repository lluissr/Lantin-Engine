#ifndef __ModuleFileSystem_h__
#define __ModuleFileSystem_h__

#include "Module.h"
#include <string>
#include <map>
#include <vector>

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem();
	~ModuleFileSystem();

	bool Init() override;

	bool AddPath(const char* path);
	bool FileExists(const char* path, const char* atDirectory = nullptr, const char* withExtension = nullptr);

	unsigned ReadFile(const char* path, char** buffer);
	unsigned WriteFile(const char* path, const void* buffer, unsigned size, bool overwrite = false);
	const char* GetAvailablePath(const char* path);
	void SplitPath(const char* full_path, std::string* path, std::string* filename, std::string* extension);
	void GetExtension(const char* full_path, std::string* extension);
	std::string NormalizePath(const char * path);
	std::map<std::string, std::string> GetFilesFromDirectoryRecursive(const char * directory);
	void GetFilesFromDirectory(const char * directory, std::vector<std::string>& fileList) const;
	void UpdateFiles();

	std::vector<std::string> meshList;
	std::vector<std::string> texturesList;
};

#endif

