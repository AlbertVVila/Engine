#include "Application.h"

#include "ModuleScript.h"
#include "ModuleScene.h"

ModuleScript::ModuleScript()
{
}


ModuleScript::~ModuleScript()
{
}


bool ModuleScript::Start()
{
	return true;
}

update_status ModuleScript::Update(float dt)
{
	return UPDATE_CONTINUE;
}
