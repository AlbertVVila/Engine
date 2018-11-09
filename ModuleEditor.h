#ifndef __ModuleEditor_h__
#define __ModuleEditor_h__

#include "Module.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

class Panel;
class PanelConsole;
class PanelScene;
class PanelProperties;
class PanelConfiguration;
class PanelAbout;

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
	void RenderGUI();
	void CreateDockSpace();
	void DrawPanels();

	bool IsCameraFocused() const;
	void AddFpsLog(float fps) const;
	void processInput(SDL_Event* event) const;

	PanelConsole *console = nullptr;
	PanelScene *scene = nullptr;	
	PanelConfiguration *configuration = nullptr;
	PanelProperties *properties = nullptr;
	PanelAbout *about = nullptr;

private:
	std::list<Panel*> panels;
};

#endif __ModuleEditor_h__
