#include "PanelAbout.h"
#include "Application.h"


PanelAbout::PanelAbout()
{
}


PanelAbout::~PanelAbout()
{
}

void PanelAbout::Draw()
{
	ImGui::Begin("About", &show);

	ImGui::Text("Name:        Lantin Engine");
	ImGui::Text("Description: Engine developed in UPC");
	ImGui::Text("Author:      Lluis Sanchez Roig");
	ImGui::Separator();
	ImGui::Text("Software");
	if (ImGui::MenuItem("SDL 2.0.9"))
	{
		ShellExecute(NULL, "open", "https://www.libsdl.org/download-2.0.php", NULL, NULL, SW_SHOWNORMAL);
	}
	if (ImGui::MenuItem("OpenGL 4"))
	{
		ShellExecute(NULL, "open", "https://www.opengl.org/", NULL, NULL, SW_SHOWNORMAL);
	}
	if (ImGui::MenuItem("glew 2.1.0"))
	{
		ShellExecute(NULL, "open", "https://github.com/nigels-com/glew", NULL, NULL, SW_SHOWNORMAL);
	}
	if (ImGui::MenuItem("Assimp 3.3.1"))
	{
		ShellExecute(NULL, "open", "https://github.com/assimp/assimp", NULL, NULL, SW_SHOWNORMAL);
	}
	if (ImGui::MenuItem("DevIL 1.8.0"))
	{
		ShellExecute(NULL, "open", "http://openil.sourceforge.net/", NULL, NULL, SW_SHOWNORMAL);
	}
	if (ImGui::MenuItem("ImGui - docking branch"))
	{
		ShellExecute(NULL, "open", "https://github.com/ocornut/imgui", NULL, NULL, SW_SHOWNORMAL);
	}
	if (ImGui::MenuItem("MathGeoLib 1.5"))
	{
		ShellExecute(NULL, "open", "https://github.com/juj/MathGeoLib", NULL, NULL, SW_SHOWNORMAL);
	}
	if (ImGui::MenuItem("mmgr 1.0"))
	{
		ShellExecute(NULL, "open", "http://www.flipcode.com/archives/Presenting_A_Memory_Manager.shtml", NULL, NULL, SW_SHOWNORMAL);
	}

	ImGui::Separator();
	if (ImGui::MenuItem("MIT License"))
	{
		ShellExecute(NULL, "open", "https://github.com/lluissr/Lantin-Engine/blob/master/LICENSE", NULL, NULL, SW_SHOWNORMAL);
	}

	ImGui::End();
}
