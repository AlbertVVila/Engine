#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleSceneLoader.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "ModuleTime.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"
#include "Timer.h"

using namespace std;

Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules.push_back(window = new ModuleWindow());
	modules.push_back(fsystem = new ModuleFileSystem());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(textures = new ModuleTextures());
	modules.push_back(input = new ModuleInput());
    modules.push_back(camera = new ModuleCamera());
	modules.push_back(sceneLoader = new ModuleSceneLoader());
	modules.push_back(program = new ModuleProgram());
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(time = new ModuleTime());
	modules.push_back(scene = new ModuleScene());
	modules.push_back(resManager = new ModuleResourceManager());
}

Application::~Application()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        RELEASE(*it);
    }
}

bool Application::Init()
{
	Timer t;
	t.Start();

	bool ret = true;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	for (list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Start();

	LOG("Init + Start time: %d ms",t.Stop());

	return ret;
}

update_status Application::Update()
{
	//Timer t;
	//t.Start();
	update_status ret = UPDATE_CONTINUE;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();
	//LOG("Update time: %d ms", t.Stop());

	editor->AddFpsLog(1 / App->time->deltaTime);
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
	{
		ret = (*it)->CleanUp();
	}
	return ret;
}
