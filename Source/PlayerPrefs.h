#ifndef __PLAYERPREFS_H__
#define __PLAYERPREFS_H__

#include <map>
#include <assert.h>

class PlayerPrefs
{
public:
	PlayerPrefs();
	~PlayerPrefs();

	static void Clear();

	static void Save();
	static void Load();
	static int			GetInt(const char* key, int defaultValue = 0);
	static float		GetFloat(const char* key, float defaultValue = 0.0f);
	static const char*	GetString(const char* key, const char* defaultValue = nullptr);
	static void			SetInt(const char* key, int value);
	static void			SetFloat(const char* key, float value);
	static void			SetString(const char* key, const char* value);

	static bool			HasKey(const char* key);

	static void			DeleteKey(const char * key);
	static void			DeleteAll();

private:
	template <class T>
	static void PersistMap(const std::map<const char*, T>& myMap, char** cursor);
	
	template <class T>
	static void RetrieveMap(std::map<const char*, T>& myMap, char** cursor);
	
	template <class T>
	static T Get(const std::map<const char*, T>& myMap, const char* key, T defaultValue);
	
	template <class T>
	static void Set(std::map<const char*, T>& myMap, const char* key, T value);
	
	template <class T>
	static void CleanMap(std::map<const char*, T>& myMap);
	template <>
	static void CleanMap(std::map<const char*, const char*>& myMap);
	
	template <class T>
	static bool Find(const std::map<const char*, T>& myMap, const char* key);

private:
	static std::map<const char*, int> integers;
	static std::map<const char*, float> floats;
	static std::map<const char*, const char*> strings;
};

template <class T>
inline void PlayerPrefs::PersistMap(const std::map<const char*, T>& myMap, char** cursor)
{
	unsigned mapSize = sizeof(myMap);
	memcpy(*cursor, &mapSize, sizeof(unsigned));
	*cursor += sizeof(unsigned);
	memcpy(*cursor, &myMap, mapSize);
	*cursor += mapSize;
}

template <class T>
inline void PlayerPrefs::RetrieveMap(std::map<const char*, T>& myMap, char** cursor)
{
	unsigned size = 0u;
	memcpy(&size, *cursor, sizeof(unsigned));
	*cursor += sizeof(unsigned);
	memcpy(&myMap, *cursor, size);
	*cursor += size;
}

template<class T>
inline T PlayerPrefs::Get(const std::map<const char*, T>& myMap, const char * key, T defaultValue)
{
	assert(key != nullptr);
	std::map<const char*, T>::const_iterator it = myMap.find(key);
	if (it != myMap.end())
	{
		return it->second;
	}
	return defaultValue;
}

template<class T>
inline void PlayerPrefs::Set(std::map<const char*, T>& myMap, const char * key, T value)
{
	DeleteKey(key);
	assert(key != nullptr);
	char* cpyKey = new char[strlen(key) + 1];
	strcpy(cpyKey, key);
	myMap.insert(std::make_pair(cpyKey, value));
}

template<class T>
inline void PlayerPrefs::CleanMap(std::map<const char*, T>& myMap)
{
	std::map<const char*, T>::iterator it;
	for (it = myMap.begin(); it != myMap.end(); it++)
	{
		delete[] it->first;
	}
}

template<>
inline void PlayerPrefs::CleanMap(std::map<const char*, const char*>& myMap)
{
	std::map<const char*, const char*>::iterator it;
	for (it = myMap.begin(); it != myMap.end(); it++)
	{
		delete[] it->first;
		delete[] it->second;
	}
}

template<class T>
inline bool PlayerPrefs::Find(const std::map<const char*, T>& myMap, const char* key)
{
	std::map<const char*, T>::const_iterator it = myMap.find(key);
	return it != myMap.end();
}

#endif __PLAYERPREFS_H__
