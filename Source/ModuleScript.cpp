#include "Application.h"

#include "ModuleScript.h"
#include "ModuleFileSystem.h"

#include "BaseScript.h"
#include <assert.h>
#include <windows.h>

typedef Script*(__cdecl *CreatePointer)();

ModuleScript::ModuleScript()
{
}

ModuleScript::~ModuleScript()
{
}


bool ModuleScript::Start()
{
	/*std::vector<std::string> */scripts = App->fsystem->ListFiles(SCRIPTS, false);
	for (const auto& script : scripts)
	{
		int time = App->fsystem->GetModTime(script.c_str());

	}
	return true;
}

update_status ModuleScript::Update(float dt)
{
	//TODO: Listen script folder for DLL updates
	//TODO: We should use a thread component to listen to folders asynchronously
	return UPDATE_CONTINUE;
}

Script* ModuleScript::AddScript(std::string script)
{
	HMODULE dll = LoadLibrary((SCRIPTS + script + DLL).c_str()); //TODO: don't load library if already script loaded
	assert(dll != nullptr);
	if (dll != nullptr)
	{
		CreatePointer Create = (CreatePointer)GetProcAddress(dll, "CreateScript");
		assert(Create != nullptr);
		if (Create != nullptr)
		{
			Script* script = Create();
			scriptInstances.push_back(script);
			return script;
		}
		//if (!FreeLibrary(dll))
		//{
		//	LOG("CAN'T RELEASE %s", script);
		//}
	}
}

void ModuleScript::RemoveScript(Script* script)
{
	//TODO: check if script is used in any other component and if not then freelibrary
	//FreeLibrary()
	scriptInstances.remove(script);
	RELEASE(script);
}

