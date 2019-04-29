#ifndef  __NPCController_h__
#define  __NPCController_h__

#include "BaseScript.h"

#ifdef NPCController_EXPORTS
#define NPCController_API __declspec(dllexport)
#else
#define NPCController_API __declspec(dllimport)
#endif

class NPCController_API NPCController : public Script
{
	void Start() override;
	void Update() override;

private:
	GameObject* player = nullptr;
	std::string playerName = "Player";
	std::string playerBboxName = "PlayerMesh";
	std::string myBboxName = "EnemyMesh";

	// BBoxes
	math::AABB* myBbox = nullptr;
	math::AABB* playerBbox = nullptr;

};

#endif __NPCController_h__
