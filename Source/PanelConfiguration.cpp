#include "PanelConfiguration.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"
#include "imgui.h"

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
		DrawMSgraph();
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
	if (ImGui::CollapsingHeader("Scene"))
	{
		if (ImGui::Button("Reset QuadTree"))
		{
			App->scene->ResetQuadTree();
		}
	}
	ImGui::End();
}

void PanelConfiguration::DrawFPSgraph() const
{
	if (fps.size() == 0) return;

	float fmin = fps[0];
	float total = fps[0];
	for (unsigned int i = 1; i < fps.size(); i++) {
		total += fps[i];
		fmin = MIN(fmin, fps[i]);
	}
	char avg[32];
	char minfps[32];
	sprintf_s(avg, "%s%.2f", "avg:", total / fps.size());
	sprintf_s(minfps, "%s%.2f", "min:", fmin);
	ImGui::PlotHistogram("FPS", &fps[0], fps.size(), 0, avg,  0.0f, 120.0f, ImVec2(0, 80));
	ImGui::Text(minfps);
}

void PanelConfiguration::DrawMSgraph() const
{
	if (ms.size() == 0) return;

	float msMAX = ms[0];
	float total = ms[0];
	for (unsigned int i = 1; i < ms.size(); i++) {
		total += ms[i];
		msMAX = MAX(msMAX, ms[i]);
	}
	char avg[32];
	char maxms[32];
	sprintf_s(avg, "%s%.2f", "avg:", total / ms.size());
	sprintf_s(maxms, "%s%.2f", "max:", msMAX);
	ImGui::PlotHistogram("MS", &ms[0], ms.size(), 0, avg, 0.0f, 5.0f, ImVec2(0, 80));
	ImGui::Text(maxms);
}

void PanelConfiguration::AddFps(float dt)
{
	fps.insert(fps.begin(), 1 / dt);
	ms.insert(ms.begin(), dt*1000);
	if (dt * 1000 > 10)
	{
		LOG("high MS: %f", dt * 1000);
	}
	if (fps.size() > NUMFPS)
	{
		fps.pop_back();
		ms.pop_back();
	}
}

void PanelConfiguration::DrawMemoryStats() const
{
	//sMStats stats = m_getMemoryStatistics();
	//ImGui::Text("Total Reported Mem: %u", stats.totalReportedMemory);
	//ImGui::Text("Total Actual Mem: %u", stats.totalActualMemory);
	//ImGui::Text("Peak Reported Mem: %u", stats.peakReportedMemory);
	//ImGui::Text("Peak Actual Mem: %u", stats.peakActualMemory);
	//ImGui::Text("Accumulated Reported Mem: %u", stats.accumulatedReportedMemory);
	//ImGui::Text("Accumulated Actual Mem: %u", stats.accumulatedActualMemory);
	//ImGui::Text("Accumulated Alloc Unit Count: %u", stats.accumulatedAllocUnitCount);
	//ImGui::Text("Total Alloc Unit Count: %u", stats.totalAllocUnitCount);
	//ImGui::Text("Peak Alloc Unit Count: %u", stats.peakAllocUnitCount);
}


