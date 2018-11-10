#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleModel.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "ModuleTime.h"
#include "Timer.h"

using namespace std;

Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules.push_back(window = new ModuleWindow());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(textures = new ModuleTextures());
	modules.push_back(input = new ModuleInput());
    modules.push_back(camera = new ModuleCamera());
	modules.push_back(model = new ModuleModel());
	modules.push_back(program = new ModuleProgram());
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(time = new ModuleTime());
}

Application::~Application()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }
}

bool Application::Init()
{
	Timer t;
	t.Start();

	bool ret = true;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	LOG("Init time: %f ms",t.Stop());
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
	editor->AddFpsLog(1/App->time->deltaTime);

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
