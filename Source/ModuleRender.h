#ifndef __ModuleRender_h__
#define __ModuleRender_h__

#include "Module.h"
class ComponentCamera;
class Skybox;

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
	void SetBlockUniforms();

private:

	void DrawGizmos() const;
	void SetViewUniform(const ComponentCamera &camera) const;
	void SetProjectionUniform(const ComponentCamera &camera) const;
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
	bool useMainCameraFrustum = false;

	Skybox* skybox = nullptr;
	

};

#endif /* __ModuleRender_h__ */
