#include "WorldControllerScript.h"

#include "Application.h"
#include "GameObject.h"
#include "ModuleNavigation.h"
#include "ComponentTransform.h"

#include "Globals.h"

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
	//we also keep track of this gameObject
	agentObjects.push_back(player->transform);
}

void WorldControllerScript::addEnemy(GameObject* enemy)
{
	int newAgentId = addAgent((float*)& enemy->transform->position);
	//once we created the agent, save the info in our map
	objectAgentIndexMap[enemy->UUID] = newAgentId;
	//we also keep track of this gameObject
	agentObjects.push_back(enemy->transform);
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

}
void WorldControllerScript::EnemyMoveRequest(unsigned int agentId)
{

}

void WorldControllerScript::Update()
{

	//parameter is 0.1 which communicates to crowd update that it has 100ms to do the stuff
	crowdToolPointer.get()->UpdateCrowd(0.1f);

	for (ComponentTransform* agent : agentObjects)
	{
		agent->gameobject->movedFlag = true;
		std::stringstream s;
		s << "Player pos = " << agent->position.x << ", " << agent->position.y << ", " << agent->position.z << std::endl;
		LOG(s.str().c_str());
	}
}
