#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
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

	//glewInit();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
																////io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
																//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

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
	//////////////////////////////No tira
//ImGui::ShowDemoWindow(&show);
//ImGui::ShowTestWindow();

//////////////////////////////Prueba 1

/*
ImGui::Begin("Another Window", &show);
ImGui::Text("Hello from another window!");
if (ImGui::Button("Close Me"))
{
	show = false;
}
ImGui::End();*/


//////////////////////////////Prueba 2
/*static float f = 0.0f;
static int counter = 0;
ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
ImGui::Checkbox("Demo Window", &show);      // Edit bools storing our window open/close state
ImGui::Checkbox("Another Window", &show);
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	counter++;
ImGui::SameLine();
ImGui::Text("counter = %d", counter);

ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
ImGui::End();*/

//Menu
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("Graphics"))
	{
		if (ImGui::MenuItem("Color"))
		{
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