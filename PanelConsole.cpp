#include "PanelConsole.h"


PanelConsole::PanelConsole()
{
	show = true;
}


PanelConsole::~PanelConsole()
{
}

void PanelConsole::Draw()
{
	ImGui::Begin("Console", &show);
	if (ImGui::Button("Clear")) Clear();
	ImGui::SameLine();
	bool copy = ImGui::Button("Copy");
	ImGui::Separator();
	ImGui::BeginChild("scrolling");
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
	if (copy) ImGui::LogToClipboard();
	ImGui::TextUnformatted(Buf.begin());
	if (ScrollToBottom)
		ImGui::SetScrollHere(1.0f);
	ScrollToBottom = false;
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::End();
}

void PanelConsole::AddLog(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	Buf.appendfv(fmt, args);
	va_end(args);
	ScrollToBottom = true;
}

void PanelConsole::Clear()
{
	Buf.clear();
}
