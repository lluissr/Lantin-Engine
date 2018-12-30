#include "PanelLibrary.h"
#include "Application.h"
#include "ModuleFileSystem.h"


PanelLibrary::PanelLibrary()
{
	show = true;
}


PanelLibrary::~PanelLibrary()
{
}

void PanelLibrary::Draw()
{
	ImGui::Begin("Library", &show);

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_None;

	bool libraryOpen = ImGui::TreeNodeEx("Library", node_flags, "Library");

	if (libraryOpen)
	{
		if (updateList)
		{
			App->fileSystem->UpdateFiles();
			updateList = false;
		}

		DrawTreeNode("Meshes", false);
		DrawTreeNode("Textures", false);

		ImGui::TreePop();
	}

	ImGui::End();

}

void PanelLibrary::DrawTreeNode(const char* name, bool isLeaf)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_None;

	if (isLeaf)
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;
	}

	bool resourceOpen = ImGui::TreeNodeEx(name, nodeFlags, name);

	PopupMenu(name);

	if (resourceOpen)
	{
		if (name == "Meshes")
		{
			for (std::vector<std::string>::iterator iterator = App->fileSystem->meshList.begin(); iterator != App->fileSystem->meshList.end(); ++iterator)
			{
				DrawTreeNode((*iterator).c_str(), true);
			}
		}
		else if (name == "Textures")
		{
			for (std::vector<std::string>::iterator iterator = App->fileSystem->texturesList.begin(); iterator != App->fileSystem->texturesList.end(); ++iterator)
			{
				DrawTreeNode((*iterator).c_str(), true);
			}
		}

		ImGui::TreePop();
	}
}

void PanelLibrary::PopupMenu(const char * name)
{
	if (name != "Meshes" && name != "Textures")
	{
		if (ImGui::IsItemClicked(1))
		{
			ImGui::OpenPopup("LibraryMenu");
		}

		if (ImGui::BeginPopup("LibraryMenu"))
		{

			if (ImGui::MenuItem("Delete"))
			{
				std::string ext;
				App->fileSystem->SplitPath(name, nullptr, nullptr, &ext);
				std::string nameToRemove = name;
				if (ext == "pisifai")
				{
					nameToRemove.insert(0, "/Library/Meshes/");
				}
				else if (ext == "dds")
				{
					nameToRemove.insert(0, "/Library/Textures/");
				}

				if (App->fileSystem->Remove(nameToRemove.c_str()))
				{
					updateList = true;
				}
			}
			ImGui::EndPopup();
		}
	}
}
