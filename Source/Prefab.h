#ifndef __Prefab_h__
#define __Prefab_h__

#include "Resource.h"

#include <vector>

class GameObject;

class Prefab : public Resource
{
public:
	Prefab(unsigned uid, GameObject* go = nullptr);
	Prefab(const Prefab& resource);
	virtual ~Prefab();

	bool LoadInMemory() override;
	void DeleteFromMemory() override;

	void AddInstance(GameObject * go);
	void RemoveInstance(GameObject * go);

	void Update(GameObject* go);
	void CheckoutPrefab();
	void Save(GameObject* go) const;

private:
	void Load(char** data);

private:
	std::vector<GameObject*> instances;
	std::string name;

public:
	GameObject* root; //version of prefab on starting engine

};

#endif __ResourcePrefab_h__