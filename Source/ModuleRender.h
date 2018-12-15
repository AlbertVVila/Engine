#ifndef __ModuleRender_h__
#define __ModuleRender_h__

#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"

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
	void SetBlockUniforms();

private:

	void DrawGizmos() const;
	void SetViewUniform(ComponentCamera *camera) const;
	void SetProjectionUniform(ComponentCamera *camera) const;
	void DrawLines() const;
	void DrawAxis() const;
	void DrawFrustum() const;
	void GenSkyBox();
	void DrawSkyBox(const ComponentCamera& camera) const;
	void InitSDL();
	void InitOpenGL() const;

public:
	void* context = nullptr;

private:
	unsigned int UBO = 0;
	unsigned int skybox_cubemap = 0;
	bool useCheckersTexture = false;
	bool depthTest = true;
	bool wireframe = false;
	bool boundingBox = false;
	bool vsync = false;

	unsigned skyboxVAO, skyboxVBO;

};

#endif /* __ModuleRender_h__ */
