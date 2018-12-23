#ifndef __Application_h__
#define __Application_h__

#include<list>
#include "Globals.h"
class Module;
class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleCamera;
class ModuleProgram;
class ModuleEditor;
class ModuleTime;
class ModuleSceneLoader;
class ModuleScene;
class ModuleFileSystem;
class ModuleResourceManager;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

public:
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleInput* input = nullptr;
    ModuleCamera* camera = nullptr;
	ModuleProgram* program = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleTime* time = nullptr;
	ModuleSceneLoader* sceneLoader = nullptr;
	ModuleScene* scene = nullptr;
	ModuleFileSystem* fsystem = nullptr;
	ModuleResourceManager* resManager = nullptr;

private:
	std::list<Module*> modules;

};

extern Application* App;

#endif //__Application_h__