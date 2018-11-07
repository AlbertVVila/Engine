#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleRenderExercise.h"
#include "Panel.h"
#include "PanelConsole.h"
#include "SDL.h"
#include "GL/glew.h"

ModuleEditor::ModuleEditor()
{
	panels.push_back(console = new PanelConsole());
}

// Destructor
ModuleEditor::~ModuleEditor()
{
}

// Called before render is available
bool ModuleEditor::Init()
{
	const char* glsl_version = "#version 130";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGui::StyleColorsDark();

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	//ImGui_ImplSDL2_ProcessEvent(&event);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	//ImGui::SetNextWindowPos({ 0,0 });
	//ImGui::SetNextWindowSize({ (float)App->renderer->width, (float)App->renderer->height });
	//ImGui::SetNextWindowBgAlpha(0.0f);

	//ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;

	//ImGui::Begin("GUI", false, window_flags);

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
				ImGui::EndFrame();
				return UPDATE_ERROR;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows"))
		{
			if (ImGui::MenuItem("Console"), NULL, console->IsEnabled())
			{
				console->SetEnabled(true);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	DrawPanels();
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	LOG("Destroying editor");
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

void ModuleEditor::RenderGUI()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	if (App->editor->io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

update_status ModuleEditor::QuitMenu()
{
	ImGui::EndMainMenuBar();
	ImGui::EndMenu();
	ImGui::End();
	ImGui::EndFrame();
	return UPDATE_STOP;
}

void ModuleEditor::DrawPanels()
{
	for (std::list<Panel*>::iterator it = panels.begin(); it != panels.end(); ++it)
		if ((*it)->IsEnabled())
		{
			(*it)->Draw();
		}
}
//void ModuleEditor::ShowGUI() const
//{
//
//	//ImGui::Text("Welcome to our GUI");
//	//ImGui::PushItemWidth(ImGui::GetFontSize() * -12);
//	if (ImGui::BeginMainMenuBar())
//	{
//		if (ImGui::BeginMenu("Menu"))
//		{
//			ShowMenu();
//			ImGui::EndMenu();
//		}
//		ImGui::EndMainMenuBar();
//	}
//
//	//ImGui::Spacing();
//
//	//SDL_version sdlVersion;
//	//SDL_GetVersion(&sdlVersion);
//
//	//if (ImGui::CollapsingHeader("Hardware"))
//	//{
//	//	//SDL_VERSION
//	//	ImGui::Text("SDL Version:"); ImGui::SameLine();
//	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d.%d.%d",sdlVersion.major, sdlVersion.minor, sdlVersion.patch);
//	//}
//
//	//if (ImGui::CollapsingHeader("About"))
//	//{
//	//	ImGui::Text("Name:"); ImGui::SameLine();
//	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), TITLE);
//	//	ImGui::Separator();
//
//	//	ImGui::Text("Description:"); ImGui::SameLine();
//	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), ENGINE_DESCRIPTION);
//	//	ImGui::Separator();
//
//	//	if (ImGui::SmallButton("Code")) {
//	//		ShellExecuteA(NULL, "open", REPOSITORY, NULL, NULL, SW_SHOWNORMAL);
//	//	};
//	//	ImGui::Separator();
//
//	//	ImGui::Text("Author:"); ImGui::SameLine();
//	//	ImGui::TextColored(ImVec4(1, 1, 0, 1), AUTHOR);
//	//	ImGui::Separator();
//
//	//	if (ImGui::TreeNode("Libraries"))
//	//	{
//	//		ImGui::BulletText("SDL (version %d.%d.%d)", sdlVersion.major, sdlVersion.minor, sdlVersion.patch);
//	//		ImGui::BulletText("Imgui (version %s)",IMGUI_VERSION);
//	//		ImGui::BulletText("MathGeoLib");
//	//		ImGui::BulletText("glew (version %d.%d)", VER_MAJORVERSION, VER_MINORVERSION);
//
//	//		ImGui::TreePop();
//	//	}
//	//	ImGui::Separator();
//	//	if (ImGui::SmallButton("License")) {
//	//		ShellExecuteA(NULL, "open", LICENSE, NULL, NULL, SW_SHOWNORMAL);
//	//	};
//	//	ImGui::Separator();
//	//}
//
//////}

//void ModuleEditor::ShowMenu() const
//{
//	ImGui::MenuItem("(dummy menu)", NULL, false, false);
//	if (ImGui::MenuItem("New")) {}
//	if (ImGui::MenuItem("Open", "Ctrl+O")) {}
//	if (ImGui::MenuItem("Quit", "Alt+F4")) {}
//}
