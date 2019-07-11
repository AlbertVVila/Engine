#include "Application.h"

#include "ModuleScript.h"
#include "ModuleFileSystem.h"
#include "ModuleTime.h"

#include "GameObject.h"
#include "Component.h"

#include "BaseScript.h"
#include "MemoryModule.h"
#include "imgui.h"
#include "PlayerPrefs.h"
#include <assert.h>
#include <windows.h>
#include <iostream>
#include "JSON.h"

typedef Script*(__cdecl *CreatePointer)();

ModuleScript::ModuleScript()
{
}

ModuleScript::~ModuleScript()
{

	for (const auto& dll : loadedDLLs)
	{
		if (!FreeLibrary(dll.second.first))
		{
			LOG("CAN'T RELEASE %s", dll.first);
		}
	}
}

bool ModuleScript::Init(JSON* config)
{
	PlayerPrefs::Load();
	SetDllDirectory(SCRIPTS);

	CheckScripts();

	JSON_value* scriptInfo = config->GetValue("scripts");
	if (scriptInfo == nullptr) return true;

	hotReloading = scriptInfo->GetInt("hotReloading");
	
	//monitorThread = std::thread(&ModuleScript::CheckScripts, this);
	//monitorThread.detach();
	//LoadFromMemory(IDR_DLL1);
	return true;
}

void ModuleScript::SaveConfig(JSON* config)
{
	JSON_value* scriptInfo = config->CreateValue();

	scriptInfo->AddInt("hotReloading", hotReloading);
	config->AddValue("scripts", *scriptInfo);
}

bool ModuleScript::CleanUp()
{
	PlayerPrefs::Save(); //Saves to Disk
	PlayerPrefs::DeleteAll(); //Deletes All memory allocated
	return true;
}

update_status ModuleScript::Update(float dt)
{
	if (dllRemoveList.size() > 0)
	{
		for (std::string name : dllRemoveList)
		{
			RemoveDLL(name);
		}
		dllRemoveList.clear();
	}
	if (App->time->gameState == GameState::RUN)
	{
		if (onStart)
		{
			for (const auto& script : componentsScript)
			{
				if (script->gameobject->isActive())
				{
 					script->Awake();
					script->hasBeenAwoken = true;
				}
			}

			for (const auto& script : componentsScript)
			{
				if (script->enabled)
				{
					script->Start();
					script->hasBeenStarted = true;
				}
			}
		}
		for (const auto& script : componentsScript)
		{
			script->Update();
		}
	}

	if (hotReloading)
	{
		CheckScripts();
	}
	if (!onStart && App->time->gameState == GameState::STOP)
	{
		ResetScriptFlags();
	}
	onStart = App->time->gameState == GameState::STOP;
	return status;
}

void ModuleScript::ResetScriptFlags()
{
	for (const auto& script : componentsScript)
	{
		script->hasBeenAwoken = false;
		script->hasBeenStarted = false;
	}
}

void ModuleScript::DrawGUI()
{
	ImGui::Checkbox("Hot Reloading", &hotReloading);
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

void ModuleScript::AddScriptReference(Script* script, const std::string&name)
{
	std::map<std::string, std::pair<HINSTANCE, int>>::iterator itDll = loadedDLLs.find(name);
	if (itDll != loadedDLLs.end())
	{
		componentsScript.push_back(script);
		itDll->second.second++;
	}
}

Script* ModuleScript::GetScript(const std::string& name)
{
	HINSTANCE dll;
	std::map<std::string, std::pair<HINSTANCE, int>>::iterator itDll = loadedDLLs.find(name);
	if (itDll != loadedDLLs.end())
	{
		dll = itDll->second.first;
		if (itDll->second.second == 0)
		{
			for (size_t i = 0; i < dllRemoveList.size(); i++)
			{
				if (name == dllRemoveList[i])
				{
					std::swap(dllRemoveList[i], dllRemoveList.back());
					dllRemoveList.pop_back();
				}
			}
		}
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
		itDll->second.second--;
		if(itDll->second.second == 0)
		{
			dllRemoveList.push_back(name);
		}
	}
	else
	{
		LOG("DLL %s Not Found!", name);
		return false;
	}
	return true;
}

bool ModuleScript::RemoveDLL(const std::string& name)
{
	std::map<std::string, std::pair<HINSTANCE, int>>::iterator itDll = loadedDLLs.find(name);
	if (itDll != loadedDLLs.end())
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
		LOG("DLL %s Not Found!", name);
		return false;
	}
	return true;
}

void ModuleScript::CheckScripts() //HOT SWAP IF USED DLL
{
	std::vector<std::string> scriptFiles = App->fsystem->GetFolderContent(SCRIPTS, true);
	std::map<std::string, int>::iterator it;

	for (const auto& scriptFile : scriptFiles)
	{
		std::string scriptName = App->fsystem->GetFilename(scriptFile);

		int time = App->fsystem->GetModTime((SCRIPTS + scriptFile).c_str());
		it = scripts.find(scriptName);
		if (it != scripts.end())
		{
			if (time > it->second)
			{
				//Update DLL but only if already loaded!!
				it->second = time;
				LOG("Script %s updated", scriptName.c_str());
				if (App->fsystem->GetExtension(scriptFile) == HOT)
				{
					HotSwap(it->first);
				}
			}
		}
		else
		{
			scripts.insert(std::pair<std::string, int>(scriptName, time));
		}
	}
}

void ModuleScript::HotSwap(std::string scriptName) //only 1 script ftm
{
	std::vector<std::pair<JSON_value*, GameObject*>> scriptInfo;

	JSON* json = new JSON();
	for (auto& script : componentsScript)
	{
		if (script->name == scriptName)
		{
			JSON_value* value = json->CreateValue();
			script->Serialize(value);
			scriptInfo.push_back(std::make_pair(value, script->gameobject));

			script->gameobject->RemoveComponent(*script);
			break;
		}
	}
	RemoveDLL(scriptName);

	bool deleted = false;
	while (!deleted)
	{
		App->fsystem->Delete((SCRIPTS + scriptName + DLL).c_str()); //Deleted old DLL
	}
	App->fsystem->Rename(SCRIPTS, (scriptName + HOT).c_str(), scriptName.c_str(), DLL); //Changed name new DLL
	HINSTANCE dll = LoadLibrary((scriptName + DLL).c_str());
	if (dll != nullptr)
	{
		loadedDLLs.insert(std::pair<std::string,
			std::pair<HINSTANCE, int>>(scriptName, std::pair<HINSTANCE, int>(dll, 1)));
	}

	assert(dll != nullptr);
	CreatePointer Create = (CreatePointer)GetProcAddress(dll, "CreateScript");
	assert(Create != nullptr);
	if (Create != nullptr)
	{
		for (const auto& info : scriptInfo)
		{
			Script* script = Create();
			script->SetApp(App);
			script->name = scriptName;
			componentsScript.push_back(script);
			script->DeSerialize(info.first);
			script->gameobject = info.second;
			info.second->components.push_back(script);
		}
	}
	//TODO: maybe it's safer to re execute here awake + start
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