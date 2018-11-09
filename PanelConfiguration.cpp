#include "PanelConfiguration.h"
#include "Application.h"
#include "ModuleTime.h"
#include "SDL.h"


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
	}
	if (ImGui::CollapsingHeader("Window"))
	{
		
	}
	if (ImGui::CollapsingHeader("Camera"))
	{

	}
	if (ImGui::CollapsingHeader("Render"))
	{

	}
	if (ImGui::CollapsingHeader("Editor"))
	{

	}
	if (ImGui::CollapsingHeader("Textures"))
	{

	}
	ImGui::End();
}

void PanelConfiguration::DrawFPSgraph()
{
	SDL_GL_SetSwapInterval(1);
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


