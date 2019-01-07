#ifndef __ModuleRender_h__
#define __ModuleRender_h__

#include "Module.h"
class ComponentCamera;
class Skybox;
struct Shader;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init() override;
	bool Start() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	void Draw(const ComponentCamera& cam, int width, int height) const;
	bool CleanUp() override;
	void OnResize();
	void DrawGUI();
	void GenBlockUniforms();
	void AddBlockUniforms(const Shader &s) const;

private:

	void DrawGizmos() const;
	void SetViewUniform(const ComponentCamera &camera) const;
	void SetProjectionUniform(const ComponentCamera &camera) const;
	void DrawGrid() const;
	void DrawAxis() const;
	void DrawFrustum() const;
	void InitSDL();
	void InitOpenGL() const;

public:
	void* context = nullptr;

	bool msaa = true;
	int msaa_level = 8;
	bool msaa_lvl_changed = false;

	bool picker_debug = false;
	bool light_debug = true;
	bool quadtree_debug = true;
	bool grid_debug = true;
	unsigned current_scale = 1;

private:
	unsigned UBO = 0;
	bool useCheckersTexture = false;
	bool depthTest = true;
	bool wireframe = false;
	bool boundingBox = false;
	bool vsync = false;
	bool useMainCameraFrustum = false;

	Skybox* skybox = nullptr;
	

};

#endif /* __ModuleRender_h__ */
