#ifndef __ModuleRenderExercise_h__
#define __ModuleRenderExercise_h__

#include "Module.h"
#include "MathGeoLib.h"

class ModuleRenderExercise : public Module
{
public:
    ModuleRenderExercise();
    ~ModuleRenderExercise();

	bool            Init    ();
	update_status   Update  ();
	bool            CleanUp ();

	void ProcessInput();

	void CreateBuffers();

	void DrawLines();
	void DrawAxis();

	void ViewMatrix();
	void ProjectionMatrix();
	void ModelTransform();

	math::float4x4 LookAt(math::float3 OBS, math::float3 VRP, math::float3 up);


private:
    unsigned vbo = 0;
	unsigned vao = 0;
	Frustum frustum;

	float3 cameraPos = float3(0, 0, 5);
	float3 cameraFront = float3(0, 0, -1);
	float3 cameraUp = float3(0, 1, 0);

	float cameraSpeed = 2.0f;
};

#endif /* __ModuleRenderExercise_h__ */
