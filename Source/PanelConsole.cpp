#include "PanelConsole.h"
#include "imgui.h"
#include "Globals.h"

PanelConsole::PanelConsole()
{
	Buf = new ImGuiTextBuffer();
}


PanelConsole::~PanelConsole()
{
	RELEASE(Buf);
}

void PanelConsole::Clear()
{
	Buf->clear(); 
	LineOffsets.clear();
}

void PanelConsole::AddLog(const char * log)
{
	Buf->appendf(log);
	ScrollToBottom = true;
}

void PanelConsole::Draw()
{
	if (!ImGui::Begin("Log", &enabled))
	{
		ImGui::End();
		return;
	}
	if (ImGui::IsWindowHovered())
	{
		ImGui::SetWindowFocus();
	}
	if (ImGui::Button("Clear")) Clear();
	ImGui::SameLine();
	bool copy = ImGui::Button("Copy");
	ImGui::SameLine();
	ImGui::Separator();
	ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
	if (copy) ImGui::LogToClipboard();

	ImGui::TextUnformatted(Buf->begin());

	if (ScrollToBottom)
	{
		ImGui::SetScrollHereY(1.0f);
	}

	ScrollToBottom = false;
	if (ForceScrollToBottom)
	{
		ScrollToBottom = true;
		ForceScrollToBottom = false;
	}
	ImGui::EndChild();
	ImGui::End();
}
