#include "Application.h"

#include "ModuleScript.h"
#include "ModuleFileSystem.h"
#include "ModuleTime.h"

#include "BaseScript.h"
#include "resource.h"
#include "MemoryModule.h"

#include <assert.h>
#include <windows.h>
#include <iostream>

typedef Script*(__cdecl *CreatePointer)();

ModuleScript::ModuleScript()
{
}

ModuleScript::~ModuleScript()
{
	for (auto& script : scriptInstances)
	{
		RELEASE(script);
	}

	for (const auto& dll : loadedDLLs)
	{
		if (!FreeLibrary(dll.second.first))
		{
			LOG("CAN'T RELEASE %s", dll.first);
		}
	}
}

bool ModuleScript::Start()
{
	CheckScripts();
	//LoadFromMemory(IDR_DLL1);
	return true;
}

update_status ModuleScript::Update(float dt)
{
	if (App->time->gameState == GameState::RUN)
	{
		if (onStart)
		{
			for (const auto& script : scriptInstances)
			{
				script->Start();
			}
		}
		for (const auto& script : scriptInstances)
		{
			script->Update();
		}
	}
	else
	{
		//TODO: We should use a thread component to listen to script folder asynchronously
		CheckScripts();
	}
	onStart = App->time->gameState != GameState::RUN;
	return UPDATE_CONTINUE;
}

void ModuleScript::LoadFromMemory(int resource) //TODO: Load from memory in shipping build
{
	HRSRC hResource = FindResourceA(nullptr, MAKEINTRESOURCEA(resource), "DLL");
	HGLOBAL hMemory = LoadResource(nullptr, hResource);

	assert(hMemory != nullptr);
	int size_bytes = SizeofResource(nullptr, hResource);
	void* ptr = LockResource(hMemory);
	assert(ptr != nullptr);
	HMEMORYMODULE dll = MemoryLoadLibrary(ptr);
	if (dll != nullptr)
	{
		CreatePointer Create = (CreatePointer)MemoryGetProcAddress(dll, "CreateScript");
		if (Create != nullptr)
		{
			Script* script = Create();
			//script->SetApp(App);
			//script->SetGameObject(App->scene->root);
			//scriptInstances.push_back(script);
			//script->Start();
			//script->Update();
		}
	}
}

Script* ModuleScript::AddScript(const std::string& script)
{
	HINSTANCE dll;
	std::map<std::string, std::pair<HINSTANCE, int>>::iterator dll_it = loadedDLLs.find(script);
	if (dll_it != loadedDLLs.end())
	{
		dll = dll_it->second.first;
		dll_it->second.second++;
	}
	else
	{
		dll = LoadLibrary((SCRIPTS + script + DLL).c_str());
		if (dll != nullptr)
		{
			loadedDLLs.insert(std::pair<std::string,
				std::pair<HINSTANCE, int>>(script, std::pair<HINSTANCE, int>(dll, 1)));
		}
		else
		{
			return nullptr;
		}
	}
	assert(dll != nullptr);
	CreatePointer Create = (CreatePointer)GetProcAddress(dll, "CreateScript");
	assert(Create != nullptr);
	if (Create != nullptr)
	{
		Script* script = Create();
		scriptInstances.push_back(script);
		return script;
	}
	return nullptr;
}

void ModuleScript::RemoveScript(const std::string& name, Script* script)
{
	//TODO: check if script is used in any other component and if not then freelibrary
	std::map<std::string, std::pair<HINSTANCE,int>>::iterator dll_it = loadedDLLs.find(name);
	if (dll_it != loadedDLLs.end())
	{
		if (dll_it->second.second <= 1)
		{
			if (!FreeLibrary(dll_it->second.first))
			{
				LOG("CAN'T RELEASE %s", name);
			}
			loadedDLLs.erase(dll_it);
		}
		else
		{
			dll_it->second.second--;
		}
		scriptInstances.remove(script);
		RELEASE(script);
	}
	else
	{
		LOG("Script %s Not Found!", name);
		assert(false);
	}
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

