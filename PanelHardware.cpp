#include "PanelHardware.h"
#include "SDL.h"
#include <string>

PanelHardware::PanelHardware()
{
}


PanelHardware::~PanelHardware()
{
}

void PanelHardware::Draw()
{
	ImGui::Begin("Hardware", &show);
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
