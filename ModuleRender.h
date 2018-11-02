#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void WindowResized(unsigned width, unsigned height);

	void ViewMatrix();
	void ProjectionMatrix();
	void ModelTransform();
	math::float4x4 LookAt(math::float3 OBS, math::float3 VRP, math::float3 up);

	void DrawLines();
	void DrawAxis();

	int width = SCREEN_WIDTH;
	int height = SCREEN_HEIGHT;

public:
	void* context;
	Frustum frustum;
	char * image = "Lenna.png";
	unsigned vbo = 0;
	unsigned vao = 0;
	unsigned texture0 = 0;

private:

};
