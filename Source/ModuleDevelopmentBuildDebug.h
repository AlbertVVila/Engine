#ifndef __ModuleDevelopmentBuildDebug_h__
#define __ModuleDevelopmentBuildDebug_h__

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
class GameObject;
class MaterialEditor;
class FileExplorer;

class ModuleDevelopmentBuildDebug:
	public Module
{
public:

	bool Init(JSON * config) override;
	update_status PreUpdate() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	void RenderGUI() const;

	void processInput(SDL_Event* event) const;


};

#endif //__ModuleEditor_h__
