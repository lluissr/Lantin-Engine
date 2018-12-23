#include "PanelEditor.h"
#include "Application.h"
#include "ModuleRender.h"



PanelEditor::PanelEditor()
{
	show = true;
}


PanelEditor::~PanelEditor()
{
}

void PanelEditor::Draw()
{
	ImGui::Begin("Editor", &show);
	ImGui::Button("Play");
	ImGui::SameLine();
	ImGui::Checkbox("Axis", &App->renderer->showAxis);
	ImGui::SameLine();
	ImGui::Checkbox("Grid", &App->renderer->showGrid);
	ImGui::End();
}
