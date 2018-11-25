#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "SDL.h"
#include <GL/glew.h> 
#include "mmgr/mmgr.h"

using namespace std;

ModuleEditor::ModuleEditor()
{
	fps_log.resize(100);
	memory_log.resize(MEMORY_LOG_SIZE);
}

// Destructor
ModuleEditor::~ModuleEditor()
{
}


bool ModuleEditor::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGui::StyleColorsDark();

	return true;
}


update_status ModuleEditor::PreUpdate()
{
	fps_log.erase(fps_log.begin());
	fps_log.push_back(App->fps);
	   
	return UPDATE_CONTINUE;
}

void ModuleEditor::InitImGuiFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	CreateDockSpace();
}
void ModuleEditor::EndImGuiFrame()
{
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ModuleEditor::CreateDockSpace() const
{
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ (float)App->window->screenWidth, (float)App->window->screenHeight });
	ImGui::SetNextWindowBgAlpha(0.0f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", NULL, window_flags);
	ImGui::PopStyleVar(3);

	ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
}

update_status ModuleEditor::Update()
{

	if (showConfiguration)
	{
		DrawConfiguration();
	}

	if (showSceneTree)
	{
		DrawSceneTree();
	}

	if (showModel)
	{
		DrawModel();
	}
	
	if (showAbout)
	{
		DrawAbout();
	}

	if (showHardware)
	{
		DrawHardware();
	}

	if (showConsole)
	{
		DrawConsole();
	}

	if (showScene)
	{
		DrawScene();
	}

	//Menu
	 if (DrawMenu()) return UPDATE_STOP;

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

void ModuleEditor::addMemory(float memory)
{
	for (unsigned int i = 0; i < MEMORY_LOG_SIZE - 1; ++i)
	{
		memory_log[i] = memory_log[i + 1];
	}

	memory_log[MEMORY_LOG_SIZE - 1] = memory;
}


void ModuleEditor::DrawScene()
{
	ImGui::Begin("Scene", &showScene, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	App->renderer->DrawCameraWindow();
	ImGui::End();
}

bool ModuleEditor::DrawMenu()
{
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Exit"))
		{
			return true;
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Engine"))
	{

		if (ImGui::MenuItem("Scene Viewport"))
		{
			showScene = !showScene;
		}
		if (ImGui::MenuItem("Objects Tree"))
		{
			showSceneTree = !showSceneTree;
		}
		if (ImGui::MenuItem("Model selected"))
		{
			showModel = !showModel;
		}
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
			ShellExecute(NULL, "open", "https://github.com/lluissr/Lantin-Engine", NULL, NULL, SW_SHOWNORMAL);
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
	return false;
}


void ModuleEditor::DrawConfiguration()
{
	ImGui::SetNextWindowSize(ImVec2(300.0f, (float)App->window->screenHeight - 217.0f), ImGuiCond_FirstUseEver);
	ImGui::Begin("Configuration", &showConfiguration);

	if (ImGui::CollapsingHeader("Application"))
	{
		char title[25];
		sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

		sMStats stats = m_getMemoryStatistics();
		addMemory((float)stats.totalReportedMemory);

		ImGui::PlotHistogram("##memory", &memory_log[0], memory_log.size(), 0, "Memory Consumption (Bytes)", 0.0f, (float)stats.peakReportedMemory * 1.2f, ImVec2(310, 100));

		ImGui::Text("Total Reported Mem: %u", stats.totalReportedMemory);
		ImGui::Text("Total Actual Mem: %u", stats.totalActualMemory);
		ImGui::Text("Peak Reported Mem: %u", stats.peakReportedMemory);
		ImGui::Text("Peak Actual Mem: %u", stats.peakActualMemory);
		ImGui::Text("Accumulated Reported Mem: %u", stats.accumulatedReportedMemory);
		ImGui::Text("Accumulated Actual Mem: %u", stats.accumulatedActualMemory);
		ImGui::Text("Accumulated Alloc Unit Count: %u", stats.accumulatedAllocUnitCount);
		ImGui::Text("Total Alloc Unit Count: %u", stats.totalAllocUnitCount);
		ImGui::Text("Peak Alloc Unit Count: %u", stats.peakAllocUnitCount);
	}


	App->camera->DrawImGui();

	App->input->DrawImGui();

	App->renderer->DrawImGui();


	if (ImGui::CollapsingHeader("Window"))
	{
		int window[2] = { App->window->screenWidth, App->window->screenHeight };
		ImGui::InputInt2("Size", window, ImGuiInputTextFlags_ReadOnly);
	}


	ImGui::End();
}


void ModuleEditor::DrawSceneTree()
{
	ImGui::SetNextWindowSize(ImVec2(350.0f, (float)App->window->screenHeight - 217.0f), ImGuiCond_FirstUseEver);
	ImGui::Begin("Scene information", &showSceneTree);
	App->scene->DrawGameObjectTreeImGui();
	ImGui::End();
}

void ModuleEditor::DrawModel()
{
	ImGui::SetNextWindowSize(ImVec2(350.0f, (float)App->window->screenHeight - 217.0f), ImGuiCond_FirstUseEver);
	ImGui::Begin("Model selected", &showModel);
	App->scene->DrawModelImGui();
	ImGui::End();
}

void ModuleEditor::DrawConsole()
{
	ImGui::SetNextWindowSize(ImVec2((float)App->window->screenWidth, 200.0f), ImGuiCond_FirstUseEver);
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


void ModuleEditor::DrawHardware()
{
	ImGui::Begin("Hardware", &showHardware);
	ImGui::Text("CPUs: %d (Cache: %dkb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
	ImGui::Text("System RAM: %dGb", SDL_GetSystemRAM() / 1000);
	std::string caps = "Caps: ";
	if (SDL_HasAVX()) { caps += "AVX, "; }
	if (SDL_HasAVX2()) { caps += "AVX2, "; }
	if (SDL_HasRDTSC()) { caps += "RDTSC, "; }
	if (SDL_HasSSE()) { caps += "SSE, "; }
	if (SDL_HasSSE2()) { caps += "SSE2, "; }
	if (SDL_HasSSE3()) { caps += "SSE3, "; }
	if (SDL_HasSSE41()) { caps += "SSE41, "; }
	if (SDL_HasSSE42()) { caps += "SSE42, "; }
	if (SDL_HasMMX()) { caps += "MMX, "; }
	ImGui::Text(caps.c_str());
	ImGui::End();
}


void ModuleEditor::DrawAbout()
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