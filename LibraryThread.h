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
	std::map<std::string, std::string> files;
	while (run)
	{
		currentFiles = App->fileSystem->GetFilesFromDirectoryRecursive("/Library/");
		files = App->fileSystem->GetFilesFromDirectoryRecursive("/Assets/");
		if (files.size() > currentFiles.size())
		{
			for (std::map<std::string, std::string>::iterator iterator = files.begin(); iterator != files.end(); ++iterator)
			{
				if (currentFiles.find((*iterator).first) == currentFiles.end())
				{
					std::string ext((*iterator).first.substr((*iterator).first.length() - 3));

					if (ext == "png" || ext == "tif" || ext == "jpg")
					{
						std::string fullPath = (*iterator).second;
						fullPath.append((*iterator).first);
						if (App->textures->Import(fullPath.c_str()))
						{
							currentFiles.insert(*iterator);
						}
					}
					else if (ext == "fbx" || ext == "FBX")
					{
						std::string fullPath = (*iterator).second;
						fullPath.append((*iterator).first);
						if (App->modelLoader->Import(fullPath.c_str()))
						{
							currentFiles.insert(*iterator);
						}
					}
				}
			}

			App->fileSystem->UpdateFiles();
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
