#include "PanelViewport.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"

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
	ImGui::SetCursorPos({ 0,0 });
	if (ImGui::IsWindowFocused())
	{
		switch (frameBuffer->frameBufferType)
		{
		case FrameBufferType::EDITOR:
			App->camera->selectedCamera = App->camera->sceneCamera;
			App->editor->editorViewportX = ImGui::GetCursorPosX() + ImGui::GetWindowPos().x;
			App->editor->editorViewportY = ImGui::GetCursorPosY() + ImGui::GetWindowPos().y;
			App->editor->overEditorViewport = ImGui::IsMouseHoveringWindow();
			break;
		case FrameBufferType::GAME:
			if (App->scene->gameCamera != nullptr)
			{
				App->camera->selectedCamera = App->scene->gameCamera->componentCamera;
			}
			App->editor->overEditorViewport = false;
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
		if (size.x != App->camera->sceneCamera->screenWidth || size.y != App->camera->sceneCamera->screenHeight)
		{
			App->camera->sceneCamera->WindowResized((unsigned)size.x, (unsigned)size.y);
		}
		
		break;
	case FrameBufferType::GAME:
		if (App->scene->gameCamera != nullptr && (size.x != App->scene->gameCamera->componentCamera->screenWidth || size.y != App->scene->gameCamera->componentCamera->screenHeight))
		{
			App->scene->gameCamera->componentCamera->WindowResized((unsigned)size.x, (unsigned)size.y);
		}
		break;
	}
	ImGui::Image((ImTextureID)frameBuffer->renderTexture, { size.x, size.y }, { 0,1 }, { 1,0 });

	if (frameBuffer->frameBufferType == FrameBufferType::EDITOR)
	{
		App->renderer->DrawImGuizmo(size.x, size.y);
	}

	ImGui::End();
}
