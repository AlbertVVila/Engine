#include "Application.h"

#include "ModuleScript.h"
#include "ModuleScene.h"
#include "BaseScript.h"
#include "windows.h"
ModuleScript::ModuleScript()
{
}


ModuleScript::~ModuleScript()
{
}


bool ModuleScript::Start()
{
	typedef Script*(__cdecl *CreatePointer)();

	HMODULE dll = LoadLibrary("Scripts/TemplateScript.dll");
	if (dll != nullptr)
	{
		CreatePointer Create = (CreatePointer)GetProcAddress(dll, "CreateScript");
		if (Create != nullptr)
		{
			Script* script = Create();
			script->Start();
		}
		if (!FreeLibrary(dll))
		{
			std::cout << "CAN'T RELEASE LIBRARY" << std::endl;
		}
	}
	return true;
}

update_status ModuleScript::Update(float dt)
{
	return UPDATE_CONTINUE;
}
