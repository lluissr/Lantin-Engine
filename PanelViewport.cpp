#include "PanelViewport.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "ModuleScene.h"

PanelViewport::PanelViewport(const char* name)
{
	show = true;
	panelName = name;
}


PanelViewport::~PanelViewport()
{
}

void PanelViewport::Draw()
{
	ImGui::Begin(panelName, &show, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoFocusOnAppearing);
	if (ImGui::IsWindowFocused())
	{
		switch (frameBuffer->frameBufferType)
		{
		case FrameBufferType::EDITOR:
			App->camera->selectedCamera = App->camera->sceneCamera;
			App->renderer->sceneFocused = true;
			break;
		case FrameBufferType::GAME:
			if (App->scene->gameCamera != nullptr)
			{
				App->camera->selectedCamera = App->scene->gameCamera->componentCamera;
				App->renderer->sceneFocused = false;
			}
			break;
		default:
			App->camera->selectedCamera = App->camera->sceneCamera;
			break;
		}
	}
	ImVec2 size = ImGui::GetWindowSize();
	switch (frameBuffer->frameBufferType)
	{
	case FrameBufferType::EDITOR:
		App->camera->sceneCamera->WindowResized((unsigned)size.x, (unsigned)size.y);
		break;
	case FrameBufferType::GAME:
		if (App->scene->gameCamera != nullptr)
		{
			App->scene->gameCamera->componentCamera->WindowResized((unsigned)size.x, (unsigned)size.y);
		}
		break;
	}
	ImGui::Image((ImTextureID)frameBuffer->renderTexture, { size.x, size.y }, { 0,1 }, { 1,0 });
	ImGui::End();
}
