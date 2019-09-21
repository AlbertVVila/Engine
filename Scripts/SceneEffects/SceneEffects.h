#ifndef  __SceneEffects_h__
#define  __SceneEffects_h__

#include "BaseScript.h"

#ifdef SceneEffects_EXPORTS
#define SceneEffects_API __declspec(dllexport)
#else
#define SceneEffects_API __declspec(dllimport)
#endif

#include "Math/float3.h"
#include <vector>

#define BAT_SPEED 450.f
#define BAT_STOP_DISTANCE 100.f

class SceneEffects_API SceneEffects : public Script
{

	enum class Behaviours
	{
		NONE,
		BATS,
		STORM
	};

	void Expose(ImGuiContext* context) override;
	void Start() override;
	void Update() override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	const char* behaviours[3]  = {"None", "Bats", "Storm" };
	const char* item_current = behaviours[0];

	Behaviours behaviour = Behaviours::NONE;
	int currentBehaviour = 0;

	//Bats stuff

	struct Bat
	{
		math::float3 destination = math::float3::zero;
		GameObject* gameobject = nullptr;
	};

	std::vector<Bat> bats;
	std::vector<math::float3> waypoints;

	int batAmount = 5;
	//Storm stuff
};


extern "C" SceneEffects_API Script* CreateScript();

#endif __SceneEffects_h__
