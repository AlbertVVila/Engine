#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "PanelConsole.h"
#include "PanelScene.h"
#include "PanelConfiguration.h"
#include "PanelProperties.h"
#include "PanelAbout.h"
#include "PanelHardware.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "imgui.h"

ModuleEditor::ModuleEditor()
{
	panels.push_back(console = new PanelConsole());
	panels.push_back(configuration = new PanelConfiguration());
	panels.push_back(properties = new PanelProperties());
	panels.push_back(about = new PanelAbout());
	panels.push_back(hardware = new PanelHardware());
	panels.push_back(scene = new PanelScene());
}

// Destructor
ModuleEditor::~ModuleEditor()
{
}

// Called before render is available
bool ModuleEditor::Init()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); 
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGui::StyleColorsDark();

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	CreateDockSpace();
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleEditor::Update()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit", "Esc"))
			{
				ImGui::EndMenu();
				ImGui::EndMainMenuBar();
				ImGui::End();
				return UPDATE_STOP;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows"))
		{
			if (ImGui::MenuItem("Console", NULL, console->IsEnabled()))
			{
				console->ToggleEnabled();
			}
			if (ImGui::MenuItem("Scene", NULL, scene->IsEnabled()))
			{
				scene->ToggleEnabled();
			}
			if (ImGui::MenuItem("Configuration", NULL, configuration->IsEnabled()))
			{
				configuration->ToggleEnabled();
			}
			if (ImGui::MenuItem("Properties", NULL, properties->IsEnabled()))
			{
				properties->ToggleEnabled();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
			{
				about->SetEnabled();
			}
			if (ImGui::MenuItem("Hardware"))
			{
				hardware->SetEnabled();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	DrawPanels();
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	LOG("Destroying editor");


	bool ret = true;

	for (std::list<Panel*>::reverse_iterator it = panels.rbegin(); it != panels.rend() && ret; ++it)
	{
		RELEASE(*it);
	}
	console = nullptr; //avoid bug when adding a log after releasing panels

	panels.clear();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return ret;
}

void ModuleEditor::RenderGUI() const
{
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ModuleEditor::CreateDockSpace() const
{
	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize({ (float)App->window->width, (float)App->window->height });
	ImGui::SetNextWindowBgAlpha(0.0f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", NULL, window_flags);
	ImGui::PopStyleVar(3);

	ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
}

void ModuleEditor::DrawPanels()
{
	for (std::list<Panel*>::iterator it = panels.begin(); it != panels.end(); ++it)
		if ((*it)->IsEnabled())
		{
			(*it)->Draw();
		}
}

bool ModuleEditor::IsCameraFocused() const
{
	if (scene != nullptr)
	{
		return scene->IsFocused();
	}
	return false;
}

void ModuleEditor::AddFpsLog(float fps) const
{
	if (configuration != nullptr)
	{
		configuration->AddFps(fps);
	}
}

void ModuleEditor::processInput(SDL_Event * event) const
{
	assert(event != NULL);
	ImGui_ImplSDL2_ProcessEvent(event);
}

void ModuleEditor::AddLog(const char *log) const
{
	assert(log != NULL);
	if (console != nullptr)
	{
		console->AddLog(log);
	}
}
