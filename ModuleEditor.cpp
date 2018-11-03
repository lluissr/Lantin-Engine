#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "SDL.h"
#include <GL/glew.h> 



ModuleEditor::ModuleEditor()
{
}

// Destructor
ModuleEditor::~ModuleEditor()
{
}


bool ModuleEditor::Init()
{

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup style
	ImGui::StyleColorsDark();

	return true;
}


update_status ModuleEditor::PreUpdate()
{
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{

	if (show)
	{
		ImGui::Begin("Configuration");
		if (ImGui::CollapsingHeader("Camera"))
		{
			float nearPlane = App->camera->frustum.nearPlaneDistance;
			float farPlane = App->camera->frustum.farPlaneDistance;
			//float aspectratio = App->camera->frustum.AspectRatio.;
			float front[3] = { App->camera->frustum.front.x, App->camera->frustum.front.y, App->camera->frustum.front.z };
			float up[3] = { App->camera->frustum.up.x, App->camera->frustum.up.y, App->camera->frustum.up.z };
			float position[3] = { App->camera->frustum.pos.x, App->camera->frustum.pos.y, App->camera->frustum.pos.z };

			ImGui::InputFloat3("Front", front);
			ImGui::InputFloat3("Up", up);
			ImGui::InputFloat3("Position", position);
			ImGui::InputFloat("Near Plane", &nearPlane);
			ImGui::InputFloat("Far Plane", &farPlane);
			//ImGui::InputFloat("Aspect Ratio", &aspectratio);

		}
		ImGui::End();
	}



//Menu
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("Graphics"))
	{
		if (ImGui::MenuItem("Configuration"))
		{
			show = !show;
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("GuiDemo"))
		{
		}
		if (ImGui::MenuItem("Documentation"))
			ShellExecute(NULL, "open", "https://github.com/lluissr", NULL, NULL, SW_SHOWNORMAL);
		if (ImGui::MenuItem("Download latest"))
			ShellExecute(NULL, "open", "www.google.com", NULL, NULL, SW_SHOWNORMAL);
		if (ImGui::MenuItem("Report a bug"))
			ShellExecute(NULL, "open", "www.google.com", NULL, NULL, SW_SHOWNORMAL);
		if (ImGui::MenuItem("About"))
			ShellExecute(NULL, "open", "www.google.com", NULL, NULL, SW_SHOWNORMAL);

		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	return UPDATE_CONTINUE;
}


bool ModuleEditor::CleanUp()
{

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}