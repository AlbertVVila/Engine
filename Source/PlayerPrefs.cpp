#include "PlayerPrefs.h"
#include "Application.h"
#include "ModuleFileSystem.h"

#define PREFSFILE PERSISTENCE "player.prefs"
std::map<const char*, int> PlayerPrefs::integers;
std::map<const char*, float> PlayerPrefs::floats;
std::map<const char*, const char*> PlayerPrefs::strings;

PlayerPrefs::PlayerPrefs()
{
}


PlayerPrefs::~PlayerPrefs()
{
}

void PlayerPrefs::Clear()
{
	CleanMap(integers);
	CleanMap(floats);
	CleanMap(strings);
}

void PlayerPrefs::Save()
{
	unsigned totalSize = sizeof(integers) + sizeof(floats) + sizeof(strings) + sizeof(unsigned) * 3;
	char *buffer = new char[totalSize];
	char* cursor = buffer;

	//copying....
	PersistMap(integers, &cursor);
	PersistMap(floats, &cursor);
	PersistMap(strings, &cursor);
	App->fsystem->Save(PREFSFILE, buffer, totalSize);
	RELEASE_ARRAY(buffer);
}

void PlayerPrefs::Load()
{
	char* data = nullptr;
	unsigned size = App->fsystem->Load(PREFSFILE, &data);
	if (size > 0)
	{
		char* cursor = data;
		RetrieveMap(integers, &cursor);
		RetrieveMap(floats, &cursor);
		RetrieveMap(strings, &cursor);
	}

	integers.insert(std::make_pair("test", 12));

	RELEASE_ARRAY(data);
}


int PlayerPrefs::GetInt(const char* key, int defaultValue)
{
	return Get(integers, key, defaultValue);
}

float PlayerPrefs::GetFloat(const char* key, float defaultValue)
{
	return Get(floats, key, defaultValue);
}

const char * PlayerPrefs::GetString(const char* key, const char* defaultValue)
{
	return Get(strings, key, defaultValue);
}

void PlayerPrefs::SetInt(const char* key, int value)
{
	Set(integers, key, value);
}

void PlayerPrefs::SetFloat(const char* key, float value)
{
	Set(floats, key, value);
}

void PlayerPrefs::SetString(const char* key, const char* value)
{
	assert(value != nullptr);
	char* cpy = new char[strlen(value) + 1];
	strcpy(cpy, value);

	Set(strings, key, (const char*)cpy);
}

bool PlayerPrefs::HasKey(const char* key)
{
	return Find(integers, key) || Find(floats, key) || Find(strings, key);
}

void PlayerPrefs::DeleteKey(const char* key)
{
	
}

void PlayerPrefs::DeleteAll()
{

}
