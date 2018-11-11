#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleRenderExercise.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "SDL.h"
#include <GL/glew.h> 

using namespace std;

ModuleEditor::ModuleEditor()
{
	fps_log.resize(100);
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
	fps_log.erase(fps_log.begin());
	fps_log.push_back(App->fps);

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{

	if (showConfiguration)
	{
		ImGui::SetNextWindowPos(ImVec2((float)App->camera->screenWidth - 300.0f, 17.0f));
		ImGui::SetNextWindowSize(ImVec2(300.0f, (float)App->camera->screenHeight - 217.0f));
		ImGui::Begin("Configuration", &showConfiguration);

		if (ImGui::CollapsingHeader("Application"))
		{
			char title[25];
			sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
			ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		}

		if (ImGui::CollapsingHeader("Camera"))
		{
			float front[3] = { App->camera->cameraFront.x, App->camera->cameraFront.y, App->camera->cameraFront.z };
			float up[3] = { App->camera->cameraUp.x, App->camera->cameraUp.y, App->camera->cameraUp.z };
			float position[3] = { App->camera->cameraPosition.x, App->camera->cameraPosition.y, App->camera->cameraPosition.z };
			float mouse[2] = { (float)App->input->GetMousePosition().x, (float)App->input->GetMousePosition().y };

			ImGui::InputFloat3("Front", front);
			ImGui::InputFloat3("Up", up);
			ImGui::InputFloat3("Position", position);
			ImGui::InputFloat("Movement Speed", &App->camera->mSpeed);
			ImGui::InputFloat("Rotation Speed", &App->camera->rSpeed);
			ImGui::InputFloat("Pitch", &App->camera->pitch);
			ImGui::InputFloat("Yaw", &App->camera->yaw);
			ImGui::InputFloat("Near Plane", &App->camera->frustum.nearPlaneDistance);
			ImGui::InputFloat("Far Plane", &App->camera->frustum.farPlaneDistance);
			ImGui::InputFloat2("Mouse position", mouse);
		}

		ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_FirstUseEver);
		if (ImGui::CollapsingHeader("Render"))
		{
			ImGui::Checkbox("Bounding Box", &App->renderer->renderBoundingBox);

			const char* items[] = { "Lenna", "Backer House", "T-Rex", "Radioactive Barrel" };
			if (ImGui::Combo("Models", &currentItemSelected, items, IM_ARRAYSIZE(items)))
			{
				if (App->modelLoader->modelRendered != currentItemSelected)
				{
					App->modelLoader->CleanModel();
					App->exercise->drawLenna = currentItemSelected == 0 ? true : false;
					App->modelLoader->ChooseModelToRender(currentItemSelected);
				}
			}
		}

		if (ImGui::CollapsingHeader("Window"))
		{
			int window[2] = { App->camera->screenWidth, App->camera->screenHeight };
			ImGui::InputInt2("Size", window, ImGuiInputTextFlags_ReadOnly);
		}

		ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_FirstUseEver);
		if (ImGui::CollapsingHeader("Textures"))
		{
			if (currentItemSelected == 0)
			{
				ImGui::Image((ImTextureID)App->exercise->texture, ImVec2(200, 200));
			}
			else
			{
				for (size_t i = 0; i < App->modelLoader->materials.size(); i++)
				{
					if (App->modelLoader->materials[i].texture0 != 0)
					{
						ImGui::Image((ImTextureID)App->modelLoader->materials[i].texture0, ImVec2(200, 200));
						std::ostringstream stringStream;
						stringStream << "Dimensions: " << std::to_string(App->modelLoader->materials[i].width) << "x" << std::to_string(App->modelLoader->materials[i].height);
						ImGui::Text(stringStream.str().c_str());
					}
				}
			}
		}

		ImGui::End();
	}

	if (showAbout)
	{
		ImGui::Begin("About", &showAbout);

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

		ImGui::Separator();
		ImGui::Text("MIT License");

		ImGui::End();
	}

	if (showHardware)
	{
		ImGui::Begin("Hardware", &showHardware);
		std::ostringstream stringStream;
		stringStream << "CPUs: " << std::to_string(SDL_GetCPUCount()) << " (Cache: " << std::to_string(SDL_GetCPUCacheLineSize()) << "kb)";
		ImGui::Text(stringStream.str().c_str());
		stringStream.str("");
		stringStream << "System RAM: " << std::to_string(SDL_GetSystemRAM() / 1000) << "Gb";
		ImGui::Text(stringStream.str().c_str());
		stringStream.str("");
		stringStream << "Caps: ";
		if (SDL_HasAVX()) { stringStream << "AVX, "; }
		if (SDL_HasAVX2()) { stringStream << "AVX2, "; }
		if (SDL_HasRDTSC()) { stringStream << "RDTSC, "; }
		if (SDL_HasSSE()) { stringStream << "SSE, "; }
		if (SDL_HasSSE2()) { stringStream << "SSE2, "; }
		if (SDL_HasSSE3()) { stringStream << "SSE3, "; }
		if (SDL_HasSSE41()) { stringStream << "SSE41, "; }
		if (SDL_HasSSE42()) { stringStream << "SSE42, "; }
		if (SDL_HasMMX()) { stringStream << "MMX, "; }
		ImGui::Text(stringStream.str().c_str());
		ImGui::End();
	}

	if (showConsole)
	{
		ImGui::SetNextWindowPos(ImVec2(0, (float)App->camera->screenHeight - 200.0f));
		ImGui::SetNextWindowSize(ImVec2((float)App->camera->screenWidth, 200.0f));
		ImGui::Begin("Console", &showConsole);
		if (ImGui::Button("Clear")) Clear();
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::Separator();
		ImGui::BeginChild("scrolling");
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
		if (copy) ImGui::LogToClipboard();
		ImGui::TextUnformatted(Buf.begin());
		if (ScrollToBottom)
			ImGui::SetScrollHere(1.0f);
		ScrollToBottom = false;
		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::End();
	}


	//Menu
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Exit"))
		{
			return UPDATE_STOP;
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Engine"))
	{
		if (ImGui::MenuItem("Configuration"))
		{
			showConfiguration = !showConfiguration;
		}
		if (ImGui::MenuItem("Console"))
		{
			showConsole = !showConsole;
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("Repository"))
		{
			ShellExecute(NULL, "open", "https://github.com/lluissr/Engine", NULL, NULL, SW_SHOWNORMAL);
		}
		if (ImGui::MenuItem("Hardware"))
		{
			showHardware = !showHardware;
		}
		if (ImGui::MenuItem("About"))
		{
			showAbout = !showAbout;
		}

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

void ModuleEditor::AddLog(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	Buf.appendfv(fmt, args);
	va_end(args);
	ScrollToBottom = true;
}

void ModuleEditor::Clear()
{
	Buf.clear();
}