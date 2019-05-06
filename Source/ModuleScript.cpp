#include "Application.h"

#include "ModuleScript.h"
#include "ModuleFileSystem.h"
#include "ModuleTime.h"

#include "Component.h"
#include "ComponentScript.h"

#include "BaseScript.h"
#include "engineResource.h"
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
	//for (auto& script : componentsScript)
	//{
	//	RELEASE(script);
	//}

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
	SetDllDirectory(SCRIPTS);
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
			for (const auto& script : componentsScript) //enable - disable events
			{
				script->Start();
			}
		}
		for (const auto& script : componentsScript)
		{
			script->Update();
		}
	}
	else
	{
		//TODO: We should use a thread component to listen to script folder asynchronously
		CheckScripts();
	}
	onStart = App->time->gameState == GameState::STOP;
	return status;
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

Script* ModuleScript::GetScript(const std::string& name)
{
	HINSTANCE dll;
	std::map<std::string, std::pair<HINSTANCE, int>>::iterator itDll = loadedDLLs.find(name);
	if (itDll != loadedDLLs.end())
	{
		dll = itDll->second.first;
		itDll->second.second++;
	}
	else
	{
		dll = LoadLibrary((name + DLL).c_str());
		if (dll != nullptr)
		{
			loadedDLLs.insert(std::pair<std::string,
				std::pair<HINSTANCE, int>>(name, std::pair<HINSTANCE, int>(dll, 1)));
		}
		else
		{
			LOG(GetLastErrorAsString().c_str());
			return nullptr;
		}
	}
	assert(dll != nullptr);
	CreatePointer Create = (CreatePointer)GetProcAddress(dll, "CreateScript");
	assert(Create != nullptr);
	if (Create != nullptr)
	{
		Script* script = Create();
		script->SetApp(App);
		script->name = name;
		componentsScript.push_back(script);
		return script;
	}
	return nullptr;
}

bool ModuleScript::RemoveScript(Script* script, const std::string& name)
{
	std::map<std::string, std::pair<HINSTANCE,int>>::iterator itDll = loadedDLLs.find(name);
	if (itDll != loadedDLLs.end())
	{
		componentsScript.remove(script);
		RELEASE(script);
		if (itDll->second.second <= 1)
		{
			if (!FreeLibrary(itDll->second.first))
			{
				LOG("CAN'T RELEASE %s", name);
				return false;
			}
			loadedDLLs.erase(itDll);
		}
		else
		{
			itDll->second.second--;
		}
	}
	else
	{
		LOG("Script %s Not Found!", name);
		return false;
	}
	return true;
}

void ModuleScript::CheckScripts()
{
	std::vector<std::string> scriptNames = App->fsystem->GetFolderContent(SCRIPTS, false);
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


std::string ModuleScript::GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}