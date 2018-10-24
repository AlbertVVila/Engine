#ifndef __ModuleEditor_h__
#define __ModuleEditor_h__

#include "Module.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

class ModuleEditor :
	public Module
{
public:
	ModuleEditor();
	~ModuleEditor();
	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

public:
	ImGuiIO io;

private:
	void ShowGUI() const;
	void ShowMenu() const;

};

#endif __ModuleEditor_h__
