#ifndef __ModuleCamera_h__
#define __ModuleCamera_h__

#include "Module.h"
class ComponentCamera;
class JSON;

class ModuleCamera : public Module
{
public:
    ModuleCamera();
    ~ModuleCamera();

	bool Init(JSON * config) override;
	void SaveConfig(JSON * config) override;
	update_status Update() override;
	bool CleanUp() override;

	void InputMove() const;
	void InputRotate() const;;
	void InputCenter() const;;
	void InputOrbit() const;;
	void InputZoom() const;;

	void DrawGUI();

public:
	ComponentCamera * editorcamera = nullptr;
};

#endif /* __ModuleCamera_h__ */
