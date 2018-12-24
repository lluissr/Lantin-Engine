#include "PanelConfiguration.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleTime.h"
#include "mmgr/mmgr.h"


PanelConfiguration::PanelConfiguration()
{
	fps_log.resize(100);
	memory_log.resize(MEMORY_LOG_SIZE);

	show = true;
}


PanelConfiguration::~PanelConfiguration()
{
}


void PanelConfiguration::Draw()
{
	ImGui::Begin("Configuration", &show);

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
	if (ImGui::CollapsingHeader("Time"))
	{
		ImGui::Text("Time since engine start: %f seconds", App->time->realTime);
		ImGui::Text("Frames since engine start: %u", App->time->realFrameCount);

		if (App->time->gameState == State::STOP) {
			ImGui::TextDisabled("Time since game start: 0.0000000 seconds");
			ImGui::TextDisabled("Frames since game start: 0");
		}
		else 
		{
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Time since game start: %f seconds", App->time->gameTime);
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Frames since game start: %u", App->time->totalFrames);
		}
	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		float front[3] = { App->camera->sceneCamera->frustum.front.x,  App->camera->sceneCamera->frustum.front.y,  App->camera->sceneCamera->frustum.front.z };
		float up[3] = { App->camera->sceneCamera->frustum.up.x,  App->camera->sceneCamera->frustum.up.y,  App->camera->sceneCamera->frustum.up.z };
		float position[3] = { App->camera->sceneCamera->frustum.pos.x,  App->camera->sceneCamera->frustum.pos.y,  App->camera->sceneCamera->frustum.pos.z };

		ImGui::InputFloat3("Front", front);
		ImGui::InputFloat3("Up", up);
		ImGui::InputFloat3("Position", position);
		ImGui::InputFloat("Movement Speed", &App->camera->sceneCamera->mSpeed);
		ImGui::InputFloat("Rotation Speed", &App->camera->sceneCamera->rSpeed);
		ImGui::InputFloat("Pitch", &App->camera->sceneCamera->pitch);
		ImGui::InputFloat("Yaw", &App->camera->sceneCamera->yaw);
		ImGui::InputFloat("Near Plane", &App->camera->sceneCamera->frustum.nearPlaneDistance);
		ImGui::InputFloat("Far Plane", &App->camera->sceneCamera->frustum.farPlaneDistance);
	}

	if (ImGui::CollapsingHeader("Input"))
	{
		float mouse[2] = { (float)App->input->GetMousePosition().x, (float)App->input->GetMousePosition().y };
		ImGui::InputFloat2("Mouse position", mouse);
	}

	if (ImGui::CollapsingHeader("Render"))
	{
		ImGui::Checkbox("Bounding Box", &App->renderer->renderBoundingBox);

		ImGui::Checkbox("Checkers texture ", &App->renderer->useCheckerTexture);

		const char* items[] = { "Backer House", "T-Rex", "Radioactive Barrel" };
		if (ImGui::Combo("Models", &App->renderer->currentItemSelected, items, IM_ARRAYSIZE(items)))
		{
			if (App->modelLoader->modelRendered != App->renderer->currentItemSelected)
			{
				App->modelLoader->CleanModel();
				App->modelLoader->ChooseModelToRender(App->renderer->currentItemSelected);
			}
		}
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		int window[2] = { App->window->screenWidth, App->window->screenHeight };
		ImGui::InputInt2("Size", window, ImGuiInputTextFlags_ReadOnly);
	}

	ImGui::End();
}


void PanelConfiguration::addMemory(float memory)
{
	for (unsigned int i = 0; i < MEMORY_LOG_SIZE - 1; ++i)
	{
		memory_log[i] = memory_log[i + 1];
	}

	memory_log[MEMORY_LOG_SIZE - 1] = memory;
}
