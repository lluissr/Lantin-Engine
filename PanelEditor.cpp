#include "PanelEditor.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTime.h"



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
	if (ImGui::ArrowButton("Play", ImGuiDir_Right)) {
		if (App->time->gameState == State::STOP) {
			App->time->Start();
		}
		else {
			App->time->Stop();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("||", { 23,19 })) {
		if (App->time->gameState == State::RUN) {
			App->time->Pause(true);
		}
		else if (App->time->gameState == State::PAUSE) {
			App->time->Pause(false);
		}
	}
	ImGui::SameLine();
	ImGui::Checkbox("Axis", &App->renderer->showAxis);
	ImGui::SameLine();
	ImGui::Checkbox("Grid", &App->renderer->showGrid);
	ImGui::SameLine();
	ImGui::Checkbox("Frustum culling", &App->renderer->frustumCulling);
	ImGui::End();
}
