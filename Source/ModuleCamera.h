#ifndef __ModuleCamera_h__
#define __ModuleCamera_h__

#include "Module.h"
#include "Math/float3.h"
class ComponentCamera;

class ModuleCamera : public Module
{
public:
    ModuleCamera();
    ~ModuleCamera();

	update_status Update();
	void DrawGUI();

public:
	ComponentCamera * editorcamera = nullptr;
};

#endif /* __ModuleCamera_h__ */
