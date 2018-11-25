#ifndef __ModuleRender_h__
#define __ModuleRender_h__

#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;
class ComponentCamera;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	bool Start() override;
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void OnResize();
	void DrawGUI();

private:

	void DrawGizmos() const;
	void SetViewUniform(ComponentCamera *camera) const;
	void SetProjectionUniform(ComponentCamera *camera) const;
	void CreateBlockUniforms();
	void DrawLines() const;
	void DrawAxis() const;
	void DrawFrustum() const;
	void InitSDL();
	void InitOpenGL() const;

public:
	void* context = nullptr;

private:
	unsigned int UBO = 0;
	bool useCheckersTexture = false;
	bool depthTest = true;
	bool wireframe = false;
	bool boundingBox = false;
	bool vsync = false;
};

#endif /* __ModuleRender_h__ */
