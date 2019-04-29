#ifndef  __ChestScript_h__
#define  __ChestScript_h__

#include "BaseScript.h"

#include "Geometry/AABB.h"

#ifdef ChestScript_EXPORTS
#define ChestScript_API __declspec(dllexport)
#else
#define ChestScript_API __declspec(dllimport)
#endif

class ChestScript_API ChestScript : public Script
{
	void Start() override;
	void Update() override;

	void Expose(ImGuiContext* context);

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

private:
	GameObject* player = nullptr;
	std::string playerName = "Player";
	std::string playerBboxName = "PlayerMesh";
	std::string myBboxName = "NPCMesh";

	// BBoxes
	math::AABB* myBbox = nullptr;
	math::AABB* playerBbox = nullptr;
};

#endif __ChestScript_h__
