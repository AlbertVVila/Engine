#ifndef  __WorldControllerScript_h__
#define  __WorldControllerScript_h__

#include "BaseScript.h"
#include <vector>

#ifdef WorldControllerScript_EXPORTS
#define WorldControllerScript_API __declspec(dllexport)
#else
#define WorldControllerScript_API __declspec(dllimport)
#endif

class WorldControllerScript_API WorldControllerScript : public Script
{
public:
	WorldControllerScript();
	~WorldControllerScript();

	void setPlayer(GameObject* player);
	void addEnemy(GameObject* enemy);

private:
	//explicitly disable copy constructor and copy assignment operator
	WorldControllerScript(const WorldControllerScript&);
	WorldControllerScript& operator=(const WorldControllerScript);

	//encapsulating stuff
	std::vector<GameObject*> enemies;
	GameObject* player = nullptr;

};

extern "C" WorldControllerScript_API Script* CreateScript();

#endif __WorldControllerScript_h__
