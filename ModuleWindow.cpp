#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "imgui.h"

ModuleWindow::ModuleWindow()
{
}

// Destructor
ModuleWindow::~ModuleWindow()
{
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
		else
		{
			//Get window surface
			
			screen_surface = SDL_GetWindowSurface(window);
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

void ModuleWindow::Resize()
{
	App->renderer->frustum.horizontalFov = 2.f * atanf(tanf(App->renderer->frustum.verticalFov * 0.5f) * ((float)App->renderer->width / (float)App->renderer->height));
}

void ModuleWindow::DrawGUI()
{
	if (!fullscreen && (ImGui::InputInt("height", &App->renderer->height,10,50) || ImGui::InputInt("width", &App->renderer->width, 10, 50)))
	{
		SDL_SetWindowSize(App->window->window, App->renderer->width, App->renderer->height);
		App->renderer->WindowResized(App->renderer->width, App->renderer->height);
	}
	if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
	{
		SDL_SetWindowBrightness(App->window->window, brightness);
	}
	if (ImGui::Checkbox("FullScreen", &fullscreen))
	{
		if (fullscreen) {
			SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN);

			SDL_DisplayMode displayMode;
			SDL_GetDesktopDisplayMode(0, &displayMode);
			SDL_SetWindowSize(App->window->window, displayMode.w, displayMode.h);
			//TODO: remember old width and height to replace it later
		}
		else
			SDL_SetWindowFullscreen(App->window->window, 0);
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Resizable", &resizable))
	{

		if (resizable)
			SDL_SetWindowResizable(App->window->window, (SDL_bool)resizable);
		else
			SDL_SetWindowResizable(App->window->window, (SDL_bool)!resizable);
	}

	ImGui::NewLine();
	if (ImGui::Checkbox("Borderless", &borderless))
	{
		if (borderless)
			SDL_SetWindowBordered(App->window->window, (SDL_bool)borderless);
		else
			SDL_SetWindowBordered(App->window->window, (SDL_bool)!borderless);
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
