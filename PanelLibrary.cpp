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
	
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	bool libraryOpen = ImGui::TreeNodeEx("Library", node_flags, "Library");

	if (libraryOpen)
	{
		DrawTreeNode("Meshes", false);
		DrawTreeNode("Textures", false);

		ImGui::TreePop();
	}

	ImGui::End();
	
}

void PanelLibrary::DrawTreeNode(const char* name, bool isLeaf)
{
	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (isLeaf)
	{
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;
	}

	bool resourceOpen = ImGui::TreeNodeEx(name, nodeFlags, name);

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
