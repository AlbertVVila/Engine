#ifndef __ModuleEditor_h__
#define __ModuleEditor_h__

#include <list>
#include "Module.h"
#include "SDL_events.h"

class Panel;
class PanelConsole;
class PanelInspector;
class PanelConfiguration;
class PanelAbout;
class PanelHardware;
class PanelHierarchy;
class PanelTime;
class PanelState;
class GameObject;
class MaterialEditor;
class FileExplorer;
class PanelAnimation;

class ModuleEditor :
	public Module
{
public:
	ModuleEditor();
	~ModuleEditor();
	bool Init(JSON * config) override;
	update_status PreUpdate() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	void RenderGUI() const;
	void ShowInspector();

	void processInput(SDL_Event* event) const;
	void AddFpsLog(float dt) const;
	void AddLog(const char *log) const;

private:
	void CreateDockSpace() const;
	void DrawPanels();
	void WindowsMenu();
	void HelpMenu();

	void GenerateGenericPopUp(const char* title, const char* text);
	void OpenGenericPopUp();

public:
	PanelConsole *console = nullptr;
	PanelConfiguration *configuration = nullptr;
	PanelInspector *inspector = nullptr;
	PanelAbout *about = nullptr;
	PanelHardware *hardware = nullptr;
	PanelHierarchy *hierarchy = nullptr;
	PanelTime *time = nullptr;
	PanelState *states = nullptr;
	PanelAnimation *animation = nullptr;

	MaterialEditor *materialEditor = nullptr;
	FileExplorer *fileExplorer = nullptr;

private:
	std::list<Panel*> panels;

	// Generic Pop-Up variables
	std::string popUpTitle;
	std::string popUpText;
	bool openGenericPopUp = false;
};

#endif //__ModuleEditor_h__
