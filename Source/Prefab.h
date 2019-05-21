#ifndef __Prefab_h__
#define __Prefab_h__

#include "Resource.h"

#include <vector>

class GameObject;
class JSON;
class JSON_value;

class Prefab : public Resource
{
public:
	Prefab(unsigned uid);
	Prefab(const Prefab& resource);
	virtual ~Prefab();

	bool LoadInMemory() override;
	void DeleteFromMemory() override;

	void SaveMetafile(const char* file) const override;
	void LoadConfigFromMeta() override;

	void AddInstance(GameObject * go);
	void RemoveInstance(GameObject * go);

	void Update(GameObject* go);
	void CheckoutPrefab();
	JSON_value * GetPrefab() const;
	void Save(GameObject* go) const;

private:
	void Load(char** data);

private:
	std::vector<GameObject*> instances;
	JSON_value* prefab = nullptr;
	JSON* prefabJson = nullptr;

public:
	GameObject* root = nullptr; //version of prefab on starting engine

};

#endif __ResourcePrefab_h__