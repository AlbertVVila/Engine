#ifndef __ModuleEditor_h__
#define __ModuleEditor_h__

#include <list>
#include "Module.h"
#include "SDL_events.h"

class Panel;
class PanelConsole;
class PanelScene;
class PanelProperties;
class PanelConfiguration;
class PanelAbout;
class PanelHardware;

class ModuleEditor :
	public Module
{
public:
	ModuleEditor();
	~ModuleEditor();
	bool Init();
	update_status PreUpdate();
	update_status Update();
	bool CleanUp();

	void RenderGUI() const;
	void CreateDockSpace() const;
	void DrawPanels();

	bool IsCameraFocused() const;
	void processInput(SDL_Event* event) const;

	void AddFpsLog(float fps) const;
	void AddLog(const char *log) const;

public:
	PanelConsole *console = nullptr;
	PanelScene *scene = nullptr;	
	PanelConfiguration *configuration = nullptr;
	PanelProperties *properties = nullptr;
	PanelAbout *about = nullptr;
	PanelHardware *hardware = nullptr;

private:
	std::list<Panel*> panels;
};

#endif //__ModuleEditor_h__
