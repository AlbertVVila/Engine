#include "PanelConfiguration.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleModel.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "mmgr.h"

#define NUMFPS 100

PanelConfiguration::PanelConfiguration()
{
}


PanelConfiguration::~PanelConfiguration()
{
}


void PanelConfiguration::Draw()
{
	if (!ImGui::Begin("Configuration", &enabled))
	{
		ImGui::End();
		return;
	}
	if (ImGui::CollapsingHeader("Application"))
	{
		DrawFPSgraph();
		DrawMemoryStats();
	}
	if (ImGui::CollapsingHeader("Window"))
	{
		App->window->DrawGUI();
	}
	if (ImGui::CollapsingHeader("Camera"))
	{
		App->camera->DrawGUI();
	}
	if (ImGui::CollapsingHeader("Render"))
	{
		App->renderer->DrawGUI();
	}
	if (ImGui::CollapsingHeader("Input"))
	{
		App->input->DrawGUI();
	}
	if (ImGui::CollapsingHeader("Textures"))
	{
		App->textures->DrawGUI();
	}
	if (ImGui::CollapsingHeader("Models"))
	{
		App->model->DrawGUI();
	}
	ImGui::End();
}

void PanelConfiguration::DrawFPSgraph() const
{
	float total = 0;
	for (int i = 0; i < fps.size(); i++) {
		total += fps[i];
	}
	char avg[32];
	sprintf(avg, "%s%.2f", "avg:", total/fps.size());
	ImGui::PlotHistogram("FPS", &fps[0], fps.size(), 0, avg,  0.0f, 120.0f, ImVec2(0, 80));
}

void PanelConfiguration::AddFps(float fps_value)
{
	fps.insert(fps.begin(), fps_value);
	if (fps.size() > NUMFPS)
	{
		fps.pop_back();
	}
}

void PanelConfiguration::DrawMemoryStats() const
{
	sMStats stats = m_getMemoryStatistics();
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


