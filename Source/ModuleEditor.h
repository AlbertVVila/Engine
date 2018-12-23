#ifndef __ModuleEditor_h__
#define __ModuleEditor_h__

#include <list>
#include "Module.h"
#include "SDL_events.h"

class Panel;
class PanelConsole;
class PanelScene;
class PanelInspector;
class PanelConfiguration;
class PanelAbout;
class PanelHardware;
class PanelHierarchy;
class PanelCamera;
class GameObject;
class ComponentMaterial;

class ModuleEditor :
	public Module
{
public:
	ModuleEditor();
	~ModuleEditor();
	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	bool CleanUp() override;

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
	PanelInspector *inspector = nullptr;
	PanelAbout *about = nullptr;
	PanelHardware *hardware = nullptr;
	PanelHierarchy *hierarchy = nullptr;
	PanelCamera *camera = nullptr;

	ComponentMaterial *newMaterial = nullptr;
private:
	std::list<Panel*> panels;
};

#endif //__ModuleEditor_h__
