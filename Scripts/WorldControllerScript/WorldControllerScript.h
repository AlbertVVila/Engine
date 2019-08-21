#ifndef  __WorldControllerScript_h__
#define  __WorldControllerScript_h__

#include "BaseScript.h"
#include <map>
#include <memory>

#ifdef WorldControllerScript_EXPORTS
#define WorldControllerScript_API __declspec(dllexport)
#else
#define WorldControllerScript_API __declspec(dllimport)
#endif

class crowdTool;

class WorldControllerScript_API WorldControllerScript : public Script
{
public:
	WorldControllerScript();
	~WorldControllerScript();

	void resetWorldAgents();
	//checks if we dont have the player and creates a new agent if not
	void setPlayer(GameObject* player);

	//checks if we dont have the enemy and creates a new agent if not
	void addEnemy(GameObject* enemy);

	//move request processing
	void moveRequest(GameObject* agent);

	//the update calls detour crowds update which updates all the positions of the agents
	void Update() override;

private:
	//explicitly disable copy constructor and copy assignment operator
	WorldControllerScript(const WorldControllerScript&);
	WorldControllerScript& operator=(const WorldControllerScript);

	//other functions
	std::unique_ptr<crowdTool> crowdToolPointer = std::make_unique<crowdTool>();

	//function that adds the agent to the crowd
	int addAgent(const float* pos);


	//private variables
	//we are gonna start with just one crowd and each agent (any enemy or player) moves alone
	//map UUID-agent index
	std::map<unsigned, int> objectAgentIndexMap;
	
	unsigned playerUID = 0u;

};

extern "C" WorldControllerScript_API Script* CreateScript();

#endif __WorldControllerScript_h__
