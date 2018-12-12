#include "PanelViewport.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"

PanelViewport::PanelViewport()
{
	show = true;
}


PanelViewport::~PanelViewport()
{
}

void PanelViewport::Draw()
{
	ImGui::Begin("Scene", &show, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImVec2 size = ImGui::GetWindowSize();
	App->camera->WindowResized((unsigned)size.x, (unsigned)size.y);
	ImGui::Image((ImTextureID)App->renderer->renderTexture, { size.x, size.y }, { 0,1 }, { 1,0 });
	ImGui::End();
}
