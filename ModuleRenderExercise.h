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

	math::float4x4 ViewMatrix();
	math::float4x4 ProjectionMatrix();
	math::float4x4 ModelMatrix();

	math::float4x4 LookAt(math::float3 OBS, math::float3 VRP, math::float3 up);


private:
    unsigned vbo        = 0;
	
};

#endif /* __ModuleRenderExercise_h__ */
