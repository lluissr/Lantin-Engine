#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTime.h"
#include "ModuleScene.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"

ModuleEditor::ModuleEditor()
{
	panelConfiguration = new PanelConfiguration();
	panelModel = new PanelModel();
	panelAbout = new PanelAbout();
	panelHardware = new PanelHardware();
	panelConsole = new PanelConsole();
	panelScene = new PanelScene();
	panelViewport = new PanelViewport("Scene");
	panelViewportGame = new PanelViewport("Game");
	panelEditor = new PanelEditor();
	panelLibrary = new PanelLibrary();
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

	panelViewport->frameBuffer = &App->renderer->frameBufferScene;
	panelViewportGame->frameBuffer = &App->renderer->frameBufferGame;

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	panelConfiguration->fps_log.erase(panelConfiguration->fps_log.begin());
	panelConfiguration->fps_log.push_back(App->time->fps);

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

	if (panelConfiguration->show)
	{
		panelConfiguration->Draw();
	}

	if (panelScene->show)
	{
		panelScene->Draw();
	}

	if (panelModel->show)
	{
		panelModel->Draw();
	}

	if (panelAbout->show)
	{
		panelAbout->Draw();
	}

	if (panelHardware->show)
	{
		panelHardware->Draw();
	}

	if (panelConsole->show)
	{
		panelConsole->Draw();
	}

	if (panelViewport->show)
	{
		panelViewport->Draw();
	}

	if (panelViewportGame->show)
	{
		panelViewportGame->Draw();
	}

	if (panelEditor->show)
	{
		panelEditor->Draw();
	}

	if (panelLibrary->show)
	{
		panelLibrary->Draw();
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

	RELEASE(panelConfiguration);
	RELEASE(panelModel);
	RELEASE(panelAbout);
	RELEASE(panelHardware);
	RELEASE(panelScene);
	RELEASE(panelViewport);
	RELEASE(panelViewportGame);
	RELEASE(panelEditor);
	RELEASE(panelLibrary);
	RELEASE(panelConsole);

	return true;
}


bool ModuleEditor::DrawMenu()
{
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Save scene"))
		{
			if (App->time->gameState == State::STOP)
			{
				App->scene->SaveSceneJSON();
			}
			else
			{
				LOG("Can save scene when game is running!!!");
			}
		}
		if (ImGui::MenuItem("Load scene"))
		{
			if (App->time->gameState == State::STOP)
			{
				App->scene->cleanScene = true;
				App->scene->loadScene = true;
			}
			else
			{
				LOG("Can load scene when game is running!!!");
			}
		}
		if (ImGui::MenuItem("Clean scene"))
		{
			if (App->time->gameState == State::STOP)
			{
				App->scene->cleanScene = true;
			}
			else
			{
				LOG("Can clean scene when game is running!!!");
			}
		}

		if (ImGui::MenuItem("Exit"))
		{
			return true;
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Engine"))
	{
		ImGui::MenuItem("Scene Viewport", "", &panelViewport->show);
		ImGui::MenuItem("Game Viewport", "", &panelViewportGame->show);
		ImGui::MenuItem("Game editor", "", &panelEditor->show);
		ImGui::MenuItem("Scene Information", "", &panelScene->show);
		ImGui::MenuItem("Model selected", "", &panelModel->show);
		ImGui::MenuItem("Configuration", "", &panelConfiguration->show);
		ImGui::MenuItem("Console", "", &panelConsole->show);
		ImGui::MenuItem("Library", "", &panelLibrary->show);
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("Repository"))
		{
			ShellExecute(NULL, "open", "https://github.com/lluissr/Lantin-Engine", NULL, NULL, SW_SHOWNORMAL);
		}
		ImGui::MenuItem("Hardware", "", &panelHardware->show);
		ImGui::MenuItem("About", "", &panelAbout->show);
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
	return false;
}