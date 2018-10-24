#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "SDL.h"
#include "GL/glew.h"


ModuleRender::ModuleRender()
{
}

// Destructor
ModuleRender::~ModuleRender()
{
}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	context = SDL_GL_CreateContext(App->window->window);

	glewInit();

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_TEXTURE_2D);

	glClearDepth(1.0f);
	glClearColor(0.3f, 0.3f, 0.3f, 1.f);

    int width, height;
   

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
																//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
																//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
	SDL_GetWindowSize(App->window->window, &width, &height);

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init("#version 130");

	// Setup style
	ImGui::StyleColorsDark();


	return true;
}

update_status ModuleRender::PreUpdate()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		if (event.type == SDL_QUIT)
			return UPDATE_STOP;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
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




	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	ImGui::Render();


	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(App->window->window);

	
	ImGui::UpdatePlatformWindows();
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");

	//Destroy window

	return true;
}

void ModuleRender::WindowResized(unsigned width, unsigned height)
{
    glViewport(0, 0, width, height); 
}

