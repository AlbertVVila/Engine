#include "Application.h"

#include "ModuleScript.h"
#include "ModuleFileSystem.h"

#include "BaseScript.h"
#include <assert.h>
#include <windows.h>
#include "resource.h"
#include "MemoryModule.h"
#include <iostream>

typedef Script*(__cdecl *CreatePointer)();

extern "C"
{
	Script* CreateScript();
}

ModuleScript::ModuleScript()
{
}

ModuleScript::~ModuleScript()
{
}


bool ModuleScript::Start()
{
	CheckScripts();
	
	//Script* script = CreateScript();
	return true;
}

update_status ModuleScript::Update(float dt)
{
	//TODO: Listen script folder for DLL updates
	//TODO: We should use a thread component to listen to folders asynchronously
	CheckScripts();
	for (const auto& script : scriptInstances)
	{
		script->Update();
	}
	//Script* playerMovement = GetProcAddress()

	return UPDATE_CONTINUE;
}

void ModuleScript::LoadFromMemory()
{
	HRSRC hResource = FindResourceA(nullptr, MAKEINTRESOURCEA(IDR_TEXT2), "TEXT");
	HGLOBAL hMemory = LoadResource(nullptr, hResource);

	int size_bytes = SizeofResource(nullptr, hResource);
	void *ptr = LockResource(hMemory);

	HMEMORYMODULE dll = MemoryLoadLibrary(ptr);
	if (dll != nullptr)
	{
		CreatePointer Create = (CreatePointer)MemoryGetProcAddress(dll, "CreateScript");
		if (Create != nullptr)
		{
			Script* script = Create();
			scriptInstances.push_back(script);
			script->Start();
			script->Update();
			int stub = script->GetWord();
			LOG("NICE");
		}
	}
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

void ModuleScript::CheckScripts()
{
	std::vector<std::string> scriptNames = App->fsystem->ListFiles(SCRIPTS, false);
	std::map<std::string, int>::iterator it;

	for (const auto& script : scriptNames)
	{
		int time = App->fsystem->GetModTime((SCRIPTS + script + DLL).c_str());
		it = scripts.find(script);
		if (it != scripts.end())
		{
			if (time > it->second)
			{
				//Update DLL but only if already loaded!!
				it->second = time;
				LOG("Scripts %s updated", script.c_str());
			}
		}
		else
		{
			scripts.insert(std::pair<std::string, int>(script, time));
		}
	}
}

