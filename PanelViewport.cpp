#include "PanelViewport.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"

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
	ImGui::Begin(panelName, &show, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImVec2 size = ImGui::GetWindowSize();
	App->camera->WindowResized((unsigned)size.x, (unsigned)size.y);
	ImGui::Image((ImTextureID)frameBuffer->renderTexture, { size.x, size.y }, { 0,1 }, { 1,0 });
	ImGui::End();
}
