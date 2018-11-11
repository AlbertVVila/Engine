#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "imgui.h"
#include "SDL.h"

ModuleWindow::ModuleWindow()
{
}

// Destructor
ModuleWindow::~ModuleWindow()
{
	RELEASE(window);
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		int width = SCREEN_WIDTH;
		int height = SCREEN_HEIGHT;
		Uint32 flags = SDL_WINDOW_SHOWN |  SDL_WINDOW_OPENGL;

		if (fullscreen)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if (resizable)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if (borderless)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if (fullscreen_desktop)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}


		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::Resize(int width, int height)
{
	this->width = width;
	this->height = height;
	App->renderer->OnResize();
}

void ModuleWindow::DrawGUI()
{
	if ((!fullscreen || !fullscreen_desktop) && (ImGui::InputInt("width", &width,10,50) ||
		ImGui::InputInt("height", &height, 10, 50)))
	{
		SDL_SetWindowSize(window, width, height);
		App->renderer->OnResize();
	}
	if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
	{
		SDL_SetWindowBrightness(App->window->window, brightness);
	}
	if (ImGui::Checkbox("FullScreen", &fullscreen))
	{
		if (fullscreen) {
			SDL_DisplayMode displayMode;
			SDL_GetDesktopDisplayMode(0, &displayMode);
			SDL_SetWindowSize(App->window->window, displayMode.w, displayMode.h);
			SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN);

			//TODO: store old width and height to replace it when !fullscreen
		}
		else
		{
			SDL_SetWindowFullscreen(App->window->window, 0);
		}
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Resizable", &resizable))
	{
		if (resizable)
			SDL_SetWindowResizable(App->window->window, (SDL_bool)true);
		else
			SDL_SetWindowResizable(App->window->window, (SDL_bool)false);
	}

	ImGui::NewLine();
	if (ImGui::Checkbox("Borderless", &borderless))
	{
		if (borderless)
			SDL_SetWindowBordered(App->window->window, (SDL_bool)false);
		else
			SDL_SetWindowBordered(App->window->window, (SDL_bool)true);
	}

	ImGui::SameLine();
	if (ImGui::Checkbox("Full Desktop", &fullscreen_desktop))
	{
		if (fullscreen_desktop) {
			SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		}
		else
			SDL_SetWindowFullscreen(App->window->window, 0);
	}
}
