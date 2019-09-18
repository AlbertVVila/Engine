#include "WorldControllerScript.h"

#include "Application.h"

#include "GameObject.h"

#include "ModuleNavigation.h"
#include "ModuleTime.h"

#include "ComponentTransform.h"

#include "Globals.h"

const math::float3 WorldControllerScript::defaultCorrection = math::float3(200.f, 200.f, 200.f);

WorldControllerScript_API Script* CreateScript()
{
	WorldControllerScript* instance = new WorldControllerScript;
	return instance;
}

WorldControllerScript::WorldControllerScript()
{
}

WorldControllerScript::~WorldControllerScript()
{
	//no need to delete crowdToolPointer since its a smart pointer and maganes itself
	objectAgentIndexMap.clear();
	agentObjects.clear();
}

void WorldControllerScript::resetWorldAgents()
{
	crowdToolPointer.reset();//delete smart pointer calling the destructor
	crowdToolPointer = std::make_unique<crowdTool>();//create a new instance
	//reset our map aswell
	objectAgentIndexMap.clear();
	agentObjects.clear();
}

int WorldControllerScript::addAgent(float* pos, float speed)
{
	return crowdToolPointer.get()->AddNewAgent(pos);
}

void WorldControllerScript::setPlayer(GameObject* player, float speed)
{
	playerUID = player->UUID;
	int newAgentId = addAgent((float*)& player->transform->position, speed);
	//once we created the agent, save the info in our map
	objectAgentIndexMap[player->UUID] = newAgentId;
	agentObjects.push_back(player);
}

void WorldControllerScript::addEnemy(GameObject* enemy)
{
	int newAgentId = addAgent((float*)& enemy->transform->position);
	//once we created the agent, save the info in our map
	objectAgentIndexMap[enemy->UUID] = newAgentId;
	agentObjects.push_back(enemy);
}

void WorldControllerScript::PlayerMoveRequest(unsigned int goID, math::float3 posY, math::float3 correction)
{
	//since its the player we have to do the ray tracing from the mouse and stuff
	//ray tracing to get the mesh point
	math::float3 destination;
	//if (!App->navigation->HighQualityMouseDetection(&destination))
	if (!App->navigation->FindIntersectionPoint(posY, destination))
	{
		LOG("could not find point intersecting the click ray during mesh collision");
		return;
	}
	//now we get the polygon reference closest to the point
	unsigned int targetRef = 0u;
	if (!App->navigation->NavMeshPolygonQuery(&targetRef, &destination, correction))
	{
		LOG("could not find point intersecting the click ray during nav query");
		return;
	}
	//we make the move request
	float* endPos = new float[3];
	endPos[0] = destination.x; endPos[1] = destination.y; endPos[2] = destination.z;
	//accquire the agent id
	int agentId = objectAgentIndexMap[goID];
	crowdToolPointer.get()->MoveRequest(agentId, targetRef, endPos);
	delete[] endPos;

}
void WorldControllerScript::EnemyMoveRequest(unsigned int agentId, math::float3 dest, math::float3 correction)
{

	unsigned int targetRef = 0u;
	if (!App->navigation->NavMeshPolygonQuery(&targetRef, &dest, correction))
	{
		LOG("could not find point intersecting the click ray during nav query");
		return;
	}
	int enemyCrowdId = objectAgentIndexMap[agentId];

	crowdToolPointer.get()->MoveRequest(enemyCrowdId, targetRef, (float*)&dest);
}

void WorldControllerScript::Update()
{
	//parameter is 0.05 which communicates to crowd update that it has 50ms to do the stuff
	crowdToolPointer.get()->UpdateCrowd(App->time->gameDeltaTime);
	for (GameObject* agent : agentObjects)
	{
		agent->movedFlag = true;
	}
}
