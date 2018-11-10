#ifndef __ModuleRender_h__
#define __ModuleRender_h__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib.h"
#include "Model.h"

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

	void ViewMatrix(unsigned int program);
	void ProjectionMatrix(unsigned int program);
	void ModelTransform(unsigned int program);
	math::float4x4 LookAt(math::float3 OBS, math::float3 VRP, math::float3 up);

	void DrawLines();
	void DrawAxis();

	void InitFrustum();
	void InitSDL();
	void InitOpenGL();

	void CreateFrameBuffer();

	void DrawGUI();

	int width = SCREEN_WIDTH;
	int height = SCREEN_HEIGHT;

public:
	void* context;
	Frustum frustum;
	unsigned int FBO,RBO;
	unsigned int renderTexture;
	bool useCheckersTexture = false;
	bool depthTest = true;
	bool wireframe = false;
	bool boundingBox = false;
};

#endif /* __ModuleRender_h__ */
