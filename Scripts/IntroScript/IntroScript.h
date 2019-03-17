#ifndef  __IntroScript_h__
#define  __IntroScript_h__

class JSON_value;
#include "BaseScript.h"
#include "Math/float3.h"

#ifdef IntroScript_EXPORTS
#define IntroScript_API __declspec(dllexport)
#else
#define IntroScript_API __declspec(dllimport)
#endif

class IntroScript_API IntroScript : public Script
{
	void Expose(ImGuiContext* context) override;

	void Start() override;
	void Update() override;

	void Serialize(JSON_value* json) const override;
	void DeSerialize(JSON_value* json) override;

	bool introDone = false;

private:
	math::float3 cameraPosition = math::float3(0, 0, 0);
	math::float3 finalPosition = math::float3(0, 0, 1000);
	float speed = 2.0f;
	float speedX = 1.0f;
	float speedY = 1.0f;
	float speedZ = 1.0f;
	float distanceToMove = 0.0f;
	float distanceMoved = 0.0f;
};

extern "C" IntroScript_API IntroScript* CreateScript();

#endif __IntroScript_h__
