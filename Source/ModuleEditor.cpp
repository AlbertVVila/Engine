#include "Application.h"

#include "ModuleEditor.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include "ModuleTextures.h"
#include "ModuleProgram.h"

#include "PanelConsole.h"
#include "PanelScene.h"
#include "PanelConfiguration.h"
#include "PanelInspector.h"
#include "PanelAbout.h"
#include "PanelHardware.h"
#include "PanelHierarchy.h"
#include "PanelCamera.h"

#include "MaterialEditor.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "imgui.h"

#include <vector>

ModuleEditor::ModuleEditor()
{
	panels.push_back(console = new PanelConsole());
	panels.push_back(configuration = new PanelConfiguration());
	panels.push_back(inspector = new PanelInspector());
	panels.push_back(about = new PanelAbout());
	panels.push_back(hardware = new PanelHardware());
	panels.push_back(scene = new PanelScene());
	panels.push_back(hierarchy = new PanelHierarchy());
	panels.push_back(camera = new PanelCamera());

	materialEditor = new MaterialEditor();
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

	//ImGui::StyleColorsDark();
	ImGuiStyle * style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	CreateDockSpace();
	ImGui::ShowDemoWindow();
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleEditor::Update()
{
	bool sceneSavePopUp = false;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::BeginMenu("Load"))
			{
				std::vector<std::string> files = App->fsystem->ListFiles(SCENES);
				for (auto &file : files)
				{
					file = App->fsystem->RemoveExtension(file.c_str());
					if (ImGui::MenuItem(file.c_str()))
					{
						App->scene->LoadScene(file.c_str());
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Save"))
			{
				if (!App->scene->name.empty())
				{
					App->scene->SaveScene(*App->scene->root, App->scene->name.c_str());
				}
				else
				{
					sceneSavePopUp = true;
				}
			}
			if (ImGui::MenuItem("Save As"))
			{
				sceneSavePopUp = true;
			}
			if (ImGui::MenuItem("Exit", "Esc"))
			{
				ImGui::EndMenu();
				ImGui::EndMainMenuBar();
				ImGui::End();
				return UPDATE_STOP;
			}
			ImGui::EndMenu();
		}
		if (sceneSavePopUp) ImGui::OpenPopup("SavePopup");
		if (ImGui::BeginPopupModal("SavePopup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Choose Scene name:\n\n");
			char name[64] = "";
			if (!App->scene->name.empty())
			{
				strcpy(name, App->scene->name.c_str());
			}
			else
			{
				strcpy(name, "Unnamed");
			}
			ImGui::InputText("name", name, 64);
			App->scene->name = name;
			ImGui::Separator();

			if (ImGui::Button("OK", ImVec2(120, 0))) {
				App->scene->SaveScene(*App->scene->root, App->scene->name.c_str());
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				sceneSavePopUp = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Empty GameObject"))
			{
				App->scene->CreateGameObject("Empty", App->scene->root);
			}
			if (ImGui::MenuItem("Sphere"))
			{
				App->scene->CreateSphere("sphere0");
			}
			if (ImGui::MenuItem("Cube"))
			{
				App->scene->CreateCube("cube0");
			}
			if (ImGui::MenuItem("New Material"))
			{
				materialEditor->open = true;
			}
			ImGui::EndMenu();
		}
		materialEditor->Draw();
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
			if (ImGui::MenuItem("Game", NULL, camera->IsEnabled()))
			{
				camera->ToggleEnabled();
			}
			if (ImGui::MenuItem("Configuration", NULL, configuration->IsEnabled()))
			{
				configuration->ToggleEnabled();
			}
			if (ImGui::MenuItem("Properties", NULL, inspector->IsEnabled()))
			{
				inspector->ToggleEnabled();
			}
			if (ImGui::MenuItem("Hierarchy", NULL, hierarchy->IsEnabled()))
			{
				hierarchy->ToggleEnabled();
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
	if (scene != nullptr && hierarchy != nullptr)
	{
		return scene->IsFocused() || hierarchy->IsFocused();
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
