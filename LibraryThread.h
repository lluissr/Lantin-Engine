#ifndef __LibraryThraed_h__
#define __LibraryThread_h__

#include <map>
#include <string>
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"

#include "thread";


bool run = true;
void Watcher()
{
	std::map<std::string, std::string> currentFiles;
	std::map<std::string, std::string> oldFiles;
	std::map<std::string, std::string> libraryFiles;
	App->fileSystem->GetFilesFromDirectoryRecursive("/Assets/", currentFiles);
	oldFiles = currentFiles;

	//Do this the first time to check and add files if new files are added while engine was not working. 
	//TODO:Improve this after integration of JSON and save settings. Now is not completly functional.
	App->fileSystem->GetFilesFromDirectoryRecursive("/Library/", libraryFiles);
	if (libraryFiles.size() < currentFiles.size())
	{
		for (std::map<std::string, std::string>::iterator iterator = currentFiles.begin(); iterator != currentFiles.end(); ++iterator)
		{
			std::string ext((*iterator).first.substr((*iterator).first.length() - 3));

			std::string fullPath = (*iterator).second;
			fullPath.append((*iterator).first);
			if (ext == "png" || ext == "tif" || ext == "jpg" || ext == "tga")
			{
				App->textures->Import(fullPath.c_str());
			}
			else if (ext == "fbx" || ext == "FBX")
			{
				App->modelLoader->Import(fullPath.c_str());
			}
		}
	}

	while (run)
	{
		currentFiles.clear();
		App->fileSystem->GetFilesFromDirectoryRecursive("/Assets/", currentFiles);
		if (currentFiles.size() > oldFiles.size())
		{
			for (std::map<std::string, std::string>::iterator iterator = currentFiles.begin(); iterator != currentFiles.end(); ++iterator)
			{
				if (oldFiles.find((*iterator).first) == oldFiles.end())
				{
					std::string ext((*iterator).first.substr((*iterator).first.length() - 3));

					std::string fullPath = (*iterator).second;
					fullPath.append((*iterator).first);
					if (ext == "png" || ext == "tif" || ext == "jpg")
					{
						App->textures->Import(fullPath.c_str());
					}
					else if (ext == "fbx" || ext == "FBX")
					{
						App->modelLoader->Import(fullPath.c_str());
					}
				}
			}

			oldFiles = currentFiles;
			App->fileSystem->UpdateFiles();
		}
		else if (currentFiles.size() < oldFiles.size())
		{
			oldFiles = currentFiles;
		}

		Sleep(1000);
	}
}

class LibraryThread
{
public:

	LibraryThread()
	{

	}

	~LibraryThread()
	{
		run = false;
	}

	void Start()
	{
		std::thread watcherThread(Watcher);

		watcherThread.detach();

		App->fileSystem->UpdateFiles();
	}

};

#endif
